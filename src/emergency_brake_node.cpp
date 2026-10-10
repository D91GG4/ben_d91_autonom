// emergency_brake_node: veszfekezo logika.
// Feliratkozik: /distance (sensor_msgs/msg/Range)
// Publikal:     /cmd_vel  (geometry_msgs/msg/Twist)  - sebessegparancs
//               /brake_status (std_msgs/msg/String)  - allapotuzenet
// Ha az akadaly a "stop_distance" kuszobnel kozelebb van, megall; a
// "slow_distance" alatt lassit, egyebkent a normal sebessegen halad.

#include <algorithm>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"

class EmergencyBrakeNode : public rclcpp::Node
{
public:
  EmergencyBrakeNode()
  : Node("emergency_brake_node")
  {
    this->declare_parameter<double>("cruise_speed", 1.0);    // [m/s]
    this->declare_parameter<double>("slow_distance", 2.0);   // [m]
    this->declare_parameter<double>("stop_distance", 1.0);   // [m]

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    status_pub_ = this->create_publisher<std_msgs::msg::String>("brake_status", 10);
    sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "distance", 10,
      std::bind(&EmergencyBrakeNode::range_callback, this, std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "emergency_brake_node elindult, feliratkozas: /distance");
  }

private:
  void range_callback(const sensor_msgs::msg::Range::SharedPtr msg)
  {
    const double cruise = this->get_parameter("cruise_speed").as_double();
    const double slow_d = this->get_parameter("slow_distance").as_double();
    const double stop_d = this->get_parameter("stop_distance").as_double();
    const double d = static_cast<double>(msg->range);

    double speed = cruise;
    std::string status = "FREE";
    if (d <= stop_d) {
      speed = 0.0;
      status = "STOP";
    } else if (d <= slow_d) {
      // linearis lassitas a ket kuszob kozott
      speed = cruise * (d - stop_d) / (slow_d - stop_d);
      speed = std::max(0.0, std::min(speed, cruise));
      status = "SLOW";
    }

    geometry_msgs::msg::Twist cmd;
    cmd.linear.x = speed;
    cmd_pub_->publish(cmd);

    std_msgs::msg::String status_msg;
    status_msg.data = status;
    status_pub_->publish(status_msg);

    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 1000,
      "tavolsag: %.2f m, sebesseg: %.2f m/s, allapot: %s", d, speed, status.c_str());
  }

  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<EmergencyBrakeNode>());
  rclcpp::shutdown();
  return 0;
}
