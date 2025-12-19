#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp" // para mensajes de velocidad
#include "sensor_msgs/msg/laser_scan.hpp" // para mensajes de escaneo láser

class RobotLaberinto : public rclcpp::Node
{
public:
    explicit RobotLaberinto(int timer_period_ms = 500, float linear_vel = 0.2, float angular_vel = 0.2)
        : Node("proyecto")
    {
        // Suscripción al tópico de escaneo láser
        scan_subscriber = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10, std::bind(&RobotLaberinto::scan_callback, this, std::placeholders::_1));
        // Publicador para comandos de velocidad
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        // Timer para publicar comandos periódicamente
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(timer_period_ms),
            std::bind(&RobotLaberinto::timer_ejecucion, this)); 
        // Inicializar parámetros
        linear_vel_ = linear_vel;
        angular_vel_ = angular_vel;
        umbral_dist_ = 2.0; // umbral de distancia para detectar obstáculos
        dist_frente_ = 10.0; // inicializar con valor alto
        // Establecer el nivel del logger a DEBUG
        this->get_logger().set_level(rclcpp::Logger::Level::Debug);                 
    }

private:
    void timer_ejecucion()
    {
        if(dist_frente_ < umbral_dist_) {
            movimiento_stop();
            // Obstáculo delante, decidir giro
            if(dist_izq_ < dist_der_) {
                movimiento_giro(false); // girar a la derecha
                RCLCPP_DEBUG(this->get_logger(), "Giro a la derecha");
            } else {
                movimiento_giro(true); // girar a la izquierda
                RCLCPP_DEBUG(this->get_logger(), "Giro a la izquierda");
            }            
        } 
        else {
            // Camino libre, avanzar
            movimiento_adelante();
            RCLCPP_DEBUG(this->get_logger(), "Avanzando");
        }
    }

    void movimiento_stop()
    {
        auto mensaje_twist = geometry_msgs::msg::Twist();
        mensaje_twist.linear.x = 0.0;
        mensaje_twist.angular.z = 0.0;
        publisher_->publish(mensaje_twist);
    }

    void movimiento_adelante()
    {
        auto mensaje_twist = geometry_msgs::msg::Twist();
        mensaje_twist.linear.x = linear_vel_;
        mensaje_twist.angular.z = 0.0;
        publisher_->publish(mensaje_twist);
    }

    void movimiento_giro(bool antihorario = true)
    {
        auto mensaje_twist = geometry_msgs::msg::Twist();
        mensaje_twist.linear.x = 0.0;
        mensaje_twist.angular.z = antihorario ? angular_vel_ : -angular_vel_;
        publisher_->publish(mensaje_twist);
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
        auto idx_right = calculate_index(msg, -90.0f);   // Derecha (-90°)

        // Almacenar las distancias medidas
        dist_frente_ = msg->ranges[idx_front];
        dist_izq_ = msg->ranges[idx_left];
        dist_der_ = msg->ranges[idx_right];

        // Imprimir las distancias medidas usando las variables almacenadas
        RCLCPP_INFO(this->get_logger(),
            "LaserScan: frente(0°)=%.2f, izquierda(90°)=%.2f, derecha(-90°)=%.2f",
            dist_frente_,
            dist_izq_,
            dist_der_
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
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_subscriber;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    // parámetros
    float linear_vel_;
    float angular_vel_;
    // variables
    float dist_frente_;
    float dist_izq_;
    float dist_der_;
    float umbral_dist_; 
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv); // Inicializar ROS2
    // parámetros del robot
    int timer_ms = 250;
    float linear_vel = 0.25;
    float angular_vel = 0.2;
    auto nodo = std::make_shared<RobotLaberinto>(timer_ms, linear_vel, angular_vel); // Crear el nodo
    while(rclcpp::ok()) {
        rclcpp::spin_some(nodo);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    rclcpp::shutdown(); // Finalizar ROS2
    return 0;
}