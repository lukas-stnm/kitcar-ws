#include <memory>
#include <cmath>
#include <limits>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/path.hpp"
#include "turtlesim/msg/pose.hpp"

using std::placeholders::_1;

class LaneKeeper : public rclcpp::Node
{
public:
  LaneKeeper()
  : Node("lane_keeper_node")
  {
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
    
    // WICHTIG: qos_profile_sensor_data hilft manchmal bei großen Datenmengen,
    // aber wir bleiben erst mal beim Standard "10".
    path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
      "/lane_boundary_middle", 10, std::bind(&LaneKeeper::path_callback, this, _1));

    pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10, std::bind(&LaneKeeper::pose_callback, this, _1));
      
    RCLCPP_INFO(this->get_logger(), "Eco-Driver gestartet! (Verarbeitet nur jede 10. Nachricht)");
  }

private:
  turtlesim::msg::Pose current_pose_;
  bool pose_received_ = false;
  
  // Zähler für den "Türsteher"
  int process_counter_ = 0;

  // Hilfsfunktion: Abstand berechnen
  double get_distance(double x1, double y1, double x2, double y2) {
      return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
  }

  void pose_callback(const turtlesim::msg::Pose::SharedPtr msg)
  {
    current_pose_ = *msg;
    pose_received_ = true;
  }

  void path_callback(const nav_msgs::msg::Path::SharedPtr msg)
  {
    // 1. Sicherheitscheck
    if (!pose_received_ || msg->poses.empty()) { return; }

    // --- NEU: DER TÜRSTEHER ---
    // Wir erhöhen den Zähler.
    process_counter_++;
    
    // Wenn der Zähler NICHT durch 10 teilbar ist -> ABBRECHEN.
    // Das heißt: Wir ignorieren 90% der Arbeit. Das rettet deinen RAM.
    if (process_counter_ % 10 != 0) {
        return; 
    }
    // ---------------------------

    // --- AB HIER: Die normale Logik (aber seltener ausgeführt) ---

    size_t closest_index = 0;
    double min_dist = std::numeric_limits<double>::max(); 

    // Wir nutzen weiterhin den "i += 20" Trick für den Loop
    for (size_t i = 0; i < msg->poses.size(); i += 20) { 
        double dist = get_distance(
            current_pose_.x, current_pose_.y,
            msg->poses[i].pose.position.x, msg->poses[i].pose.position.y
        );

        // Optimierung: Wenn wir schon sehr nah waren und uns wieder entfernen,
        // können wir die Suche abbrechen (spart Zeit bei langen Pfaden).
        // Das funktioniert aber nur, wenn der Pfad sortiert ist. 
        // Wir lassen es sicherheitshalber erst mal weg.

        if (dist < min_dist) {
            min_dist = dist;
            closest_index = i;
        }
    }

    size_t lookahead_steps = 15; 
    size_t target_index = closest_index + lookahead_steps;
    
    if (target_index >= msg->poses.size()) {
        target_index = msg->poses.size() - 1;
    }

    double target_x = msg->poses[target_index].pose.position.x;
    double target_y = msg->poses[target_index].pose.position.y;

    double dx = target_x - current_pose_.x;
    double dy = target_y - current_pose_.y;
    
    double angle_to_target = std::atan2(dy, dx);
    double angle_error = angle_to_target - current_pose_.theta;
    
    double steering_angle = std::atan2(std::sin(angle_error), std::cos(angle_error));

    auto drive_msg = geometry_msgs::msg::Twist();
    drive_msg.linear.x = 1.5; 
    drive_msg.angular.z = 3.0 * steering_angle;

    publisher_->publish(drive_msg);
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr path_sub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LaneKeeper>());
  rclcpp::shutdown();
  return 0;
}