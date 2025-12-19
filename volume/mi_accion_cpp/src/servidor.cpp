#include <memory>
#include <thread>
#include <chrono>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/action/rotate_absolute.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "tf2/LinearMath/Quaternion.hpp"
#include "tf2/LinearMath/Matrix3x3.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

// Para simplificar nombres largos
using namespace std::chrono_literals;
using RotateAbsolute = turtlesim::action::RotateAbsolute;
using GoalHandleRotateAbsolute = rclcpp_action::ServerGoalHandle<RotateAbsolute>;

// Nodo que implementa action server con goal RotateAbsolute para orientar al robot un determinado ángulo[°] odométrico
class RotateAbsoluteActionServer : public rclcpp::Node
{
public:
    RotateAbsoluteActionServer()
    : Node("RotateAbsolute_action_server"), orientacion_grados_(0.0)
    {
        // Crear el action server con callbacks para goal, cancel y aceptado
        action_server_ = rclcpp_action::create_server<RotateAbsolute>(
            this,
            "RotateAbsolute",
            std::bind(&RotateAbsoluteActionServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&RotateAbsoluteActionServer::handle_cancel, this, std::placeholders::_1),
            std::bind(&RotateAbsoluteActionServer::handle_accepted, this, std::placeholders::_1)
        );
        RCLCPP_INFO(this->get_logger(), "Action Server RotateAbsolute está listo.");
        // Suscripción al odom para obtener la orientación actual del robot
        odom_subscriber = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10, std::bind(&RotateAbsoluteActionServer::odom_callback, this, std::placeholders::_1));
        // Publicador para enviar velocidad angular al robot
        cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    }

private:
    // Callback para decidir si aceptar o rechazar un goal entrante
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const RotateAbsolute::Goal> goal)
    {
        (void)uuid;
        RCLCPP_INFO(this->get_logger(), "Recibido Goal: theta=%.2f", goal->theta);
        // Aceptar y ejecutar el goal
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    // Callback cuando el cliente solicita cancelar el goal
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleRotateAbsolute> goal_handle)
    {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(), "Solicitud de cancelación recibida.");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // Al aceptar un goal, se lanza la ejecución en otro hilo para no bloquear callbacks
    void handle_accepted(const std::shared_ptr<GoalHandleRotateAbsolute> goal_handle)
    {
        std::thread{std::bind(&RotateAbsoluteActionServer::execute, this, std::placeholders::_1), goal_handle}.detach();
    }
    // Nota: en este ejemplo no se protege acceso a variables compartidas entre hilos

    // Función que ejecuta la lógica de rotación hasta alcanzar la orientación objetivo
    void execute(const std::shared_ptr<GoalHandleRotateAbsolute> goal_handle)
    {
        RCLCPP_INFO(this->get_logger(), "Ejecutando goal...");
        auto goal = goal_handle->get_goal();
        double orientacion_deseada_grados = goal->theta; // GOAL recibido en grados
        double orientacion_inicial = orientacion_grados_; // almacenar orientación inicial para resultado
        const double tolerancia = 2.0; // tolerancia en grados
        const double velocidad_angular = 0.20; // velocidad angular usada (rad/s)
        
        rclcpp::Rate rate(10); // bucle a 10 Hz
        while (rclcpp::ok()) {
            // Si el cliente pide cancelar, detener el robot
            if (goal_handle->is_canceling()) {
                auto twist = geometry_msgs::msg::Twist();
                cmd_vel_publisher_->publish(twist); // mandar cero para detener
                // devolución del resultado
                auto result = std::make_shared<RotateAbsolute::Result>();
                result->delta = std::abs(orientacion_grados_ - orientacion_inicial);
                goal_handle->canceled(result);
                RCLCPP_INFO(this->get_logger(), "Goal cancelado.");
                return;
            }
            
            // Calcular error en grados entre objetivo y orientación actual
            double error_grados = orientacion_deseada_grados - orientacion_grados_;
            // Normalizar error a rango [-180, 180] para girar por la dirección más corta
            while (error_grados > 180.0) error_grados -= 360.0;
            while (error_grados < -180.0) error_grados += 360.0;
            
            // Si estamos dentro de la tolerancia, detener y marcar succeeded
            if (std::abs(error_grados) < tolerancia) {
                auto twist = geometry_msgs::msg::Twist();
                cmd_vel_publisher_->publish(twist); // detener robot
                // devolución del resultado
                auto result = std::make_shared<RotateAbsolute::Result>();
                result->delta = std::abs(orientacion_grados_ - orientacion_inicial);
                goal_handle->succeed(result);
                RCLCPP_INFO(this->get_logger(), "Goal alcanzado! Delta: %.2f°", result->delta);
                return;
            }
            
            // Publicar velocidad angular en la dirección del error
            auto twist = geometry_msgs::msg::Twist();
            twist.angular.z = (error_grados > 0) ? velocidad_angular : -velocidad_angular;
            cmd_vel_publisher_->publish(twist);
            // Enviar feedback con el error restante
            auto feedback = std::make_shared<RotateAbsolute::Feedback>();
            feedback->remaining = error_grados;
            goal_handle->publish_feedback(feedback);
            RCLCPP_INFO(this->get_logger(), "error_grados: %.2f°, Actual: %.2f°", error_grados, orientacion_grados_);
            
            rate.sleep();
        }
    }

    // Callback de odometría: convierte orientación quaternion a grados (yaw)
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        const auto &orientacion = msg->pose.pose.orientation;
        tf2::Quaternion q(orientacion.x, orientacion.y, orientacion.z, orientacion.w);
        double roll, pitch, yaw;
        tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);
        // Guardar orientación en grados (yaw)
        orientacion_grados_ = yaw * (180.0 / M_PI);
    }

    double orientacion_grados_; // orientación actual en grados (yaw)
    rclcpp_action::Server<RotateAbsolute>::SharedPtr action_server_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscriber;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto nodo = std::make_shared<RotateAbsoluteActionServer>();
    rclcpp::spin(nodo);
    rclcpp::shutdown();
    return 0;
}