#include <rclcpp/rclcpp.hpp>

class LoggingEjemplo : public rclcpp::Node
{
public:
    LoggingEjemplo() : Node("logging_ejemplo_cpp")
    {
        // Establecer el nivel del logger a DEBUG
        this->get_logger().set_level(rclcpp::Logger::Level::Debug);

        RCLCPP_INFO(this->get_logger(), "El nodo ha sido inicializado.");

        // Ejemplo de logging en diferentes niveles
        loggingMensajes();
    }

private:
    void loggingMensajes()
    {
        RCLCPP_DEBUG(this->get_logger(), "Este es un mensaje DEBUG.");
        RCLCPP_INFO(this->get_logger(), "Este es un mensaje INFO.");
        RCLCPP_WARN(this->get_logger(), "Este es un mensaje WARN.");
        RCLCPP_ERROR(this->get_logger(), "Este es un mensaje ERROR.");
        RCLCPP_FATAL(this->get_logger(), "Este es un mensaje FATAL.");
    }
};

int main(int argc, char **argv)
{
    // Inicializar el sistema ROS 2
    rclcpp::init(argc, argv);

    // Crear el nodo y ejecutarlo
    auto nodo = std::make_shared<LoggingEjemplo>();
    rclcpp::spin(nodo);

    // Apagar el sistema ROS 2
    rclcpp::shutdown();
    return 0;
}