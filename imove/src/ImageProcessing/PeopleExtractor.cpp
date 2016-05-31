//
// Created by Wouter Posdijk on 11/05/16.
//

#include "PeopleExtractor.h"

PeopleExtractor::PeopleExtractor(cv::Size frame_size, float pixels_per_meter, float resolution_resize_height) {
  // Calculate resize ratio
  resize_ratio = frame_size.height/resolution_resize_height;
  std::cout << pixels_per_meter << std::endl;

  // Initialize empty frame
  frame = cv::Mat::zeros(resolution_resize_height, frame_size.width/resize_ratio, CV_8UC1);

  frame_size_resized = cv::Size(frame_size.width/resize_ratio, resolution_resize_height);

  if (pixels_per_meter > 400) {
    // Initialize Detector with low camera if meter > 400 pixels
    detector = PeopleDetector(pixels_per_meter/resize_ratio, true);
  } else {
    // Initialize Detector with high camera if meter < 400 pixels
    detector = PeopleDetector(pixels_per_meter/resize_ratio, false);
  }

  // Initialize Identifier
  identifier = PeopleIdentifier(frame_size_resized.height, frame_size_resized.width);
}

PeopleExtractor::~PeopleExtractor() {}

vector<Person> PeopleExtractor::extractPeople(cv::Mat& new_frame) {
  // Convert frame to grayscale
  cvtColor(new_frame, new_frame, CV_RGB2GRAY);
  // Downscale frame
  resize(new_frame, new_frame, frame_size_resized);

  // Start working with new frame
  frame = new_frame;
  // Get a vector with every Person in the Scene, generated by the Identifier from the location provided by the Detector
  vector<Vector2> locations = detector.detect(frame);
  vector<Person> people = identifier.match(locations);

  results_frame = detector.getDisplayFrame();

  // Rescale location of every person based on downscaling
  for (Person& p : people) {
    Vector2 location = p.getLocation();
    cv::putText(results_frame, std::to_string(p.getId()), cv::Point(location.x, location.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255));
    p.setLocation(Vector2(location.x*resize_ratio,location.y*resize_ratio));
  }

  cv::line(results_frame, cv::Point(frame_size_resized.width/20, frame_size_resized.height/20), cv::Point((frame_size_resized.width - frame_size_resized.width/20), frame_size_resized.height/20), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point((frame_size_resized.width - frame_size_resized.width/20), frame_size_resized.height/20), cv::Point((frame_size_resized.width - frame_size_resized.width/20), frame_size_resized.height - frame_size_resized.height/20), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point((frame_size_resized.width - frame_size_resized.width/20), frame_size_resized.height - frame_size_resized.height/20), cv::Point(frame_size_resized.width/20, frame_size_resized.height - frame_size_resized.height/20), cv::Scalar(0, 255, 0));
  cv::line(results_frame, cv::Point(frame_size_resized.width/20, frame_size_resized.height - frame_size_resized.height/20), cv::Point(frame_size_resized.width/20, frame_size_resized.height/20), cv::Scalar(0, 255, 0));

  // Return vector containing all people in the Scene
  return people;
}

void PeopleExtractor::displayResults() {
  cv::imshow("Frame", results_frame);
}
