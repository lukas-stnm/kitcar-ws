#include <memory>
#include <cmath> // Für sin/cos
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/path.hpp"
#include "turtlesim/msg/pose.hpp" // Wir brauchen die Pose der Turtle!

using std::placeholders::_1;

class LaneKeeper : public rclcpp::Node
{
public:
  LaneKeeper()
  : Node("lane_keeper_node")
  {
    // 1. Publisher für Fahrbefehle
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

    // 2. Subscriber für den Pfad (Welt-Koordinaten)
    path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
      "/lane_boundary_middle", 10, std::bind(&LaneKeeper::path_callback, this, _1));

    // 3. NEU: Subscriber für die eigene Position (Wo bin ich?)
    pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10, std::bind(&LaneKeeper::pose_callback, this, _1));
      
    RCLCPP_INFO(this->get_logger(), "Fahrer bereit! Warte auf Pfad und Pose...");
  }

private:
  // Speichert die aktuelle Position der Turtle
  turtlesim::msg::Pose current_pose_;
  bool pose_received_ = false;

  void pose_callback(const turtlesim::msg::Pose::SharedPtr msg)
  {
    current_pose_ = *msg;
    pose_received_ = true;
  }

  void path_callback(const nav_msgs::msg::Path::SharedPtr msg)
  {
    // Ohne eigene Position können wir nicht rechnen
    if (!pose_received_ || msg->poses.empty()) {
        return; 
    }

    // Zielpunkt auswählen (Lookahead)
    size_t lookahead_index = 5;
    if (msg->poses.size() <= lookahead_index) {
        lookahead_index = msg->poses.size() - 1;
    }

    // Die Position des Zielpunkts (im Welt-System)
    double target_x = msg->poses[lookahead_index].pose.position.x;
    double target_y = msg->poses[lookahead_index].pose.position.y;

    // --- TRANSFORMATION ---
    // Wir rechnen den Punkt relativ zur Turtle um.
    // 1. Verschiebung (Vektor zum Punkt)
    double dx = target_x - current_pose_.x;
    double dy = target_y - current_pose_.y;

    // 2. Drehung (Rotieren um den eigenen Winkel negativ)
    // Formel: y_local = -dx * sin(theta) + dy * cos(theta)
    // Das gibt uns den Abstand "links/rechts" von der Turtle nase.
    double error_y_local = -dx * std::sin(current_pose_.theta) + dy * std::cos(current_pose_.theta);

    // --- REGELUNG ---
    auto drive_msg = geometry_msgs::msg::Twist();
    
    drive_msg.linear.x = 2.0;

    // P-Regler: Jetzt ist error_y_local wirklich "links/rechts von mir"
    // Positiver Fehler (links) -> Positiv lenken (links) -> POSITIVES KP
    float kp = 2.0; 
    drive_msg.angular.z = kp * error_y_local; 

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