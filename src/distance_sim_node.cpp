// distance_sim_node: szimulalt tavolsagszenzor.
// Egy "kozeledo, majd tavolodo" akadalyt szimulal: a tavolsag egy szinuszos
// hullamot koveti, ezert idonkent a veszfekezesi kuszob ala esik.
// Publikal: /distance (sensor_msgs/msg/Range)

#include <chrono>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

using namespace std::chrono_literals;

class DistanceSimNode : public rclcpp::Node
{
public:
  DistanceSimNode()
  : Node("distance_sim_node"), t_(0.0)
  {
    this->declare_parameter<double>("min_distance", 0.3);   // [m]
    this->declare_parameter<double>("max_distance", 5.0);   // [m]
    this->declare_parameter<double>("period", 10.0);        // [s] egy teljes ciklus

    publisher_ = this->create_publisher<sensor_msgs::msg::Range>("distance", 10);
    timer_ = this->create_wall_timer(100ms, std::bind(&DistanceSimNode::timer_callback, this));
    RCLCPP_INFO(this->get_logger(), "distance_sim_node elindult, publikalas: /distance");
  }

private:
  void timer_callback()
  {
    const double d_min = this->get_parameter("min_distance").as_double();
    const double d_max = this->get_parameter("max_distance").as_double();
    const double period = this->get_parameter("period").as_double();

    const double mid = 0.5 * (d_max + d_min);
    const double amp = 0.5 * (d_max - d_min);
    const double distance = mid + amp * std::sin(2.0 * M_PI * t_ / period);
    t_ += 0.1;

    sensor_msgs::msg::Range msg;
    msg.header.stamp = this->now();
    msg.header.frame_id = "sensor_link";
    msg.radiation_type = sensor_msgs::msg::Range::ULTRASOUND;
    msg.field_of_view = 0.5f;
    msg.min_range = static_cast<float>(d_min);
    msg.max_range = static_cast<float>(d_max);
    msg.range = static_cast<float>(distance);

    publisher_->publish(msg);
  }

  rclcpp::Publisher<sensor_msgs::msg::Range>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double t_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DistanceSimNode>());
  rclcpp::shutdown();
  return 0;
}
