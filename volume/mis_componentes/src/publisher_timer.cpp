#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

namespace mis_componentes
{
// 1. el constructor recibe NodeOptions
class RobotMotionPublisher : public rclcpp::Node
{
public:
    explicit RobotMotionPublisher(const rclcpp::NodeOptions & options)
        : Node("publisher_timer", options)
    {
        int timer_period_ms = 500;        
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(timer_period_ms),
            std::bind(&RobotMotionPublisher::timer_ejecucion, this));
        timer_periodo_s_ = timer_period_ms / 1000.0;
        tiempo_ = 0.0;
    }

private:
    void timer_ejecucion()
    {
        auto mensaje_twist = geometry_msgs::msg::Twist();
        mensaje_twist.linear.x = 0.2;
        if (static_cast<int>((tiempo_ / 22.0)) % 2 == 0) {
            mensaje_twist.angular.z = 0.2;
        } else {
            mensaje_twist.angular.z = -0.2;
        }
        RCLCPP_INFO(this->get_logger(), "Publicando: linear.x=%.2f, angular.z=%.2f (tiempo=%.2fs)",
                mensaje_twist.linear.x, mensaje_twist.angular.z, tiempo_);
        publisher_->publish(mensaje_twist);
        tiempo_ += timer_periodo_s_;
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    double timer_periodo_s_;
    double tiempo_;
};    

} // namespace mis_componentes

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(mis_componentes::RobotMotionPublisher)
// 2. removemos el main ya que este nodo se cargará como componente