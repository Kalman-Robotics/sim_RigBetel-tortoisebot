#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp" // para mensajes de odometría
#include "tf2/LinearMath/Quaternion.hpp" // para manejar quaternions
#include "tf2/LinearMath/Matrix3x3.hpp" // para conversiones de quaternions a Euler
#include "sensor_msgs/msg/laser_scan.hpp" // para mensajes de escaneo láser

class RobotSensorsSubscriber : public rclcpp::Node
{
public:
    RobotSensorsSubscriber()
        : Node("subscriber_sensors")
    {
        // Suscripción al tópico de odometría
        odom_subscriber = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10, std::bind(&RobotSensorsSubscriber::odom_callback, this, std::placeholders::_1));

        // Suscripción al tópico de escaneo láser
        scan_subscriber = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10, std::bind(&RobotSensorsSubscriber::scan_callback, this, std::placeholders::_1));
    }

private:
    // Callback para procesar mensajes de odometría
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        double x = msg->pose.pose.position.x; // posición x
        double y = msg->pose.pose.position.y; // posición y
        const auto &orientation = msg->pose.pose.orientation; // orientación (cuaternión)
        tf2::Quaternion q(
            orientation.x,
            orientation.y,
            orientation.z,
            orientation.w
        );
        double roll, pitch, yaw;
        tf2::Matrix3x3(q).getRPY(roll, pitch, yaw); // conversión a ángulos de Euler

        // Imprimir posición y orientación (yaw en grados)
        RCLCPP_INFO(this->get_logger(),
            "Posición: [x: %.2f, y: %.2f] | Orientación: [yaw: %.2f°]", x, y, yaw * (180.0 / M_PI));
    }

    // Callback para procesar mensajes de escaneo láser
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        auto num_ranges = msg->ranges.size(); // número de mediciones
        if (num_ranges == 0) {
            RCLCPP_WARN(this->get_logger(), "LaserScan: No se ha recibido data.");
            return;
        }
        
        // Calcular los índices para las direcciones principales
        auto idx_front = calculate_index(msg, 0.0f);     // Frente (0°)
        auto idx_left = calculate_index(msg, 90.0f);     // Izquierda (90°)
        auto idx_back = calculate_index(msg, 180.0f);    // Atrás (180°)
        auto idx_right = calculate_index(msg, -90.0f);   // Derecha (-90°)

        // Imprimir las distancias medidas en las direcciones principales
        RCLCPP_INFO(this->get_logger(),
            "LaserScan: frente(0°)=%.2f, izquierda(90°)=%.2f, atrás(180°)=%.2f, derecha(-90°)=%.2f, [#rayos=%zu]",
            msg->ranges[idx_front],
            msg->ranges[idx_left],
            msg->ranges[idx_back],
            msg->ranges[idx_right],
            num_ranges
        );
    }

    // Calcula el índice del rayo láser correspondiente a un ángulo dado (en grados)
    size_t calculate_index(const sensor_msgs::msg::LaserScan::SharedPtr msg, float angle_deg)
    {
        float angle_rad = angle_deg * M_PI / 180.0f; // conversión a radianes
        int idx = static_cast<int>(std::round((angle_rad - msg->angle_min) / msg->angle_increment));
        auto num_ranges = msg->ranges.size();

        // Limitar el índice al rango válido
        if (idx < 0) idx = 0;
        if (static_cast<size_t>(idx) >= num_ranges) idx = num_ranges - 1;

        return static_cast<size_t>(idx);
    }

    // Suscriptores a los tópicos
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscriber;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_subscriber;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv); // Inicializar ROS2
    auto nodo = std::make_shared<RobotSensorsSubscriber>(); // Crear el nodo
    rclcpp::spin(nodo); // Ejecutar el nodo
    rclcpp::shutdown(); // Finalizar ROS2
    return 0;
}