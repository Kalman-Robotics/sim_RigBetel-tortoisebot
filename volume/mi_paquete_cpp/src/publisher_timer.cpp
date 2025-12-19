#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

class RobotMotionPublisher : public rclcpp::Node
{
public:
    explicit RobotMotionPublisher(int timer_period_ms = 500)
        : Node("publisher_timer")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(timer_period_ms),
            std::bind(&RobotMotionPublisher::timer_ejecucion, this));
        timer_periodo_s_ = timer_period_ms / 1000;
        tiempo_ = 0;
    }

private:
    void timer_ejecucion()
    {
        auto mensaje_twist = geometry_msgs::msg::Twist();
        mensaje_twist.linear.x = 0.2;
        if ((tiempo_ / 22) % 2 == 0) {
            mensaje_twist.angular.z = 0.2;
        } else {
            mensaje_twist.angular.z = -0.2;
        }
        RCLCPP_INFO(this->get_logger(), "Publicando: linear.x=%.2f, angular.z=%.2f (tiempo=%ds)",
                mensaje_twist.linear.x, mensaje_twist.angular.z, tiempo_);
        publisher_->publish(mensaje_twist);
        tiempo_ += timer_periodo_s_;
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    int timer_periodo_s_;
    int tiempo_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto nodo = std::make_shared<RobotMotionPublisher>(1000);
    // rclcpp::spin(nodo);
    while(rclcpp::ok()) {
        rclcpp::spin_some(nodo);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    rclcpp::shutdown();
    return 0;
}
