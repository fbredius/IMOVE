#include "PeopleExtractor.h"

// PeopleExtractor::PeopleExtractor(const cv::Size& frame_size, float pixels_per_meter, float resolution_resize_height, const Boundary& boundary) {
PeopleExtractor::PeopleExtractor(CameraConfiguration* camConfig) {
  // Get values from camera configuration
  cv::Size frame_size = camConfig->getResolution();
  float pixels_per_meter = camConfig->getMeter();
  Boundary boundary = camConfig->getProjection().createReorientedTopLeftBoundary();

  // Height to which the frame needs to be downsized to speed up processing
  float resolution_resize_height = 216;

  // Calculate resize ratio
  resize_ratio = frame_size.height/resolution_resize_height;

  // Initialize empty frame
  frame = cv::Mat::zeros(resolution_resize_height, frame_size.width/resize_ratio, CV_8UC1);
  // Calculate frame size after resizing
  frame_size_resized = cv::Size(frame_size.width/resize_ratio, resolution_resize_height);

  // Initialize Detector
  detector = PeopleDetector(pixels_per_meter/resize_ratio, camConfig->getMinBlobArea(), camConfig->getMinBlobDistance());

  // Initialize projector boundary
  Boundary proj_bound = Boundary(Vector2(boundary.getUpperLeft().x/resize_ratio, boundary.getUpperLeft().y/resize_ratio),
                            Vector2(boundary.getUpperRight().x/resize_ratio, boundary.getUpperRight().y/resize_ratio),
                            Vector2(boundary.getLowerLeft().x/resize_ratio, boundary.getLowerLeft().y/resize_ratio),
                            Vector2(boundary.getLowerRight().x/resize_ratio, boundary.getLowerRight().y/resize_ratio));

  // Initialize Identifier
  identifier = PeopleIdentifier(proj_bound);
}

PeopleExtractor::~PeopleExtractor() {}

const scene_interface::People PeopleExtractor::extractPeople(cv::Mat& new_frame) {
  // Convert frame to grayscale
  cvtColor(new_frame, new_frame, CV_RGB2GRAY);
  // Downscale frame
  resize(new_frame, new_frame, frame_size_resized);

  // Start working with new frame
  frame = new_frame;
  // Get a vector with every Person in the Scene, generated by the Identifier from the locations provided by the Detector
  std::vector<Vector2> locations = detector.detect(frame);
  std::vector<Person> people = identifier.match(locations);

  debug_frame = detector.getDisplayFrame();

  // Rescale location of every person based on downscaling
  for (Person& p : people) {
    Vector2 location = p.getLocation();
    cv::putText(debug_frame, std::to_string(p.getId()), cv::Point(location.x, location.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255));
    p.setLocation(Vector2(location.x*resize_ratio,location.y*resize_ratio));
  }

  // Convert people to Person class from scene interface
  scene_interface::People converted_people = convert(people);
  // Return all extracted people
  return converted_people;
}

const scene_interface::People PeopleExtractor::convert(std::vector<Person>& people) {
  scene_interface::People interface_people;
  for (Person person : people) {
    // Check person type
	  scene_interface::Person::PersonType interface_person_type;
      switch (person.person_type) {
		  case Person::PersonType::Bystander:
			  interface_person_type = scene_interface::Person::PersonType::Bystander;
			  break;
		  case Person::PersonType::Participant:
			  interface_person_type = scene_interface::Person::PersonType::Participant;
			  break;
		  case Person::PersonType::None:
			  interface_person_type = scene_interface::Person::PersonType::None;
			  break;
      }
    // Check movement type
	  scene_interface::Person::MovementType interface_movement_type;
      switch (person.movement_type) {
		  case Person::MovementType::StandingStill:
			  interface_movement_type = scene_interface::Person::MovementType::StandingStill;
			  break;
		  case Person::MovementType::Moving:
			  interface_movement_type = scene_interface::Person::MovementType::Moving;
			  break;
      }

		Vector2 location = person.getLocation();
    // Add converted person to interface_people
	  interface_people.push_back(scene_interface::Person(
	      person.getId(),
	      scene_interface::Location(
					location.x,
					location.y
				),
	      interface_person_type,
	      interface_movement_type
	  ));
  }
  return interface_people;
}

const cv::Mat PeopleExtractor::getDebugFrame() const {
  return debug_frame;
}
