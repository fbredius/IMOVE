#include "PeopleExtractor.h"

PeopleExtractor::PeopleExtractor(cv::Size frame_size, float pixels_per_meter, float resolution_resize_height, Boundary boundary) {
  // Calculate resize ratio
  resize_ratio = frame_size.height/resolution_resize_height;

  // Initialize empty frame
  frame = cv::Mat::zeros(resolution_resize_height, frame_size.width/resize_ratio, CV_8UC1);

  frame_size_resized = cv::Size(frame_size.width/resize_ratio, resolution_resize_height);

  if (pixels_per_meter/frame_size.height > 0.2) {
    // Initialize Detector with low camera if meter/frame height > 0.2
    detector = PeopleDetector(pixels_per_meter/resize_ratio, true);
  } else {
    // Initialize Detector with high camera if meter < 400 pixels
    detector = PeopleDetector(pixels_per_meter/resize_ratio, false);
  }

  // Initialize Identifier
  float boundary_edge = (pixels_per_meter/resize_ratio)*1.5;

  Boundary frame_bound = Boundary(Vector2(boundary_edge, boundary_edge),
                            Vector2(frame_size_resized.width - boundary_edge, boundary_edge),
                            Vector2(boundary_edge, frame_size_resized.height - boundary_edge),
                            Vector2(frame_size_resized.width - boundary_edge, frame_size_resized.height - boundary_edge));
  Boundary proj_bound = Boundary(Vector2(boundary.getUpperLeft().x/resize_ratio, boundary.getUpperLeft().y/resize_ratio),
                            Vector2(boundary.getUpperRight().x/resize_ratio, boundary.getUpperRight().y/resize_ratio),
                            Vector2(boundary.getLowerLeft().x/resize_ratio, boundary.getLowerLeft().y/resize_ratio),
                            Vector2(boundary.getLowerRight().x/resize_ratio, boundary.getLowerRight().y/resize_ratio));

  identifier = PeopleIdentifier(proj_bound, frame_bound);
}

PeopleExtractor::~PeopleExtractor() {}

const scene_interface::People PeopleExtractor::extractPeople(cv::Mat& new_frame) {
  // Convert frame to grayscale
  cvtColor(new_frame, new_frame, CV_RGB2GRAY);
  // Downscale frame
  resize(new_frame, new_frame, frame_size_resized);

  // Start working with new frame
  frame = new_frame;
  // Get a vector with every Person in the Scene, generated by the Identifier from the location provided by the Detector
  std::vector<Vector2> locations = detector.detect(frame);
  std::vector<Person> people = identifier.match(locations);

  results_frame = detector.getDisplayFrame();

  // Rescale location of every person based on downscaling
  for (Person& p : people) {
    Vector2 location = p.getLocation();
    cv::putText(results_frame, std::to_string(p.getId()), cv::Point(location.x, location.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255));
    p.setLocation(Vector2(location.x*resize_ratio,location.y*resize_ratio));
  }

  // Return vector containing all people in the Scene according to the interface
  return this->convert(people);
}

const scene_interface::People PeopleExtractor::convert(std::vector<Person> people) {
  scene_interface::People interface_people;
  for (Person person : people) {
	  scene_interface::Person::PersonType interface_person_type;
      switch (person.person_type) {
		  case Person::PersonType::Bystander:
			  interface_person_type = scene_interface::Person::PersonType::Bystander;
			  break;
		  case Person::PersonType::Participant:
			  interface_person_type = scene_interface::Person::PersonType::Participant;
			  break;
		  case Person::PersonType::Passerthrough:
			  interface_person_type = scene_interface::Person::PersonType::Passerthrough;
			  break;
		  case Person::PersonType::None:
			  interface_person_type = scene_interface::Person::PersonType::None;
			  break;
      }
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
  cv::line(results_frame, cv::Point(36,36), cv::Point(347,36), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point(347,36), cv::Point(347,179), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point(347,179), cv::Point(36,179), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point(36,179), cv::Point(36,36), cv::Scalar(0, 255, 0));
  return results_frame;
}
