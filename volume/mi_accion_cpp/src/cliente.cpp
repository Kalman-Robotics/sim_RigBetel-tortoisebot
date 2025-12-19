#include <memory>
#include <queue>
#include "rclcpp/rclcpp.hpp"
#include "turtlesim/action/rotate_absolute.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

// Para simplificar nombres largos
using RotateAbsolute = turtlesim::action::RotateAbsolute;
using GoalHandleRotateAbsolute = rclcpp_action::ClientGoalHandle<RotateAbsolute>;

// Nodo que implementa action cliente  con goal RotateAbsolute para que el robot se oriente odométricamente una cantidad de grados 
// El cliente maneja una cola de GOALs (theta) y los envía secuencialmente al server
class RotateAbsoluteClient : public rclcpp::Node
{
public:
    explicit RotateAbsoluteClient()
    : Node("RotateAbsolute_action_client"), goal_en_progreso_(false)
    {
        // Crear cliente de acción asociado al nombre "RotateAbsolute"
        this->action_client_ = rclcpp_action::create_client<RotateAbsolute>(this, "RotateAbsolute");
    }

    // Añade un GOAL a la cola, y si no hay un goal en progreso, inicia el procesamiento
    void queue_goal(float theta)
    {
        RCLCPP_INFO(this->get_logger(), "Poniendo en cola el goal: theta=%.2f", theta);
        cola_de_goals.push(theta);
        // Si no hay goal en ejecución, comenzar a procesar el siguiente
        if (!goal_en_progreso_) {
            procesar_siguiente_goal();
        }
    }

    // Devuelve el número GOALs de la cola
    size_t obtener_tamano_cola() const
    {
        return cola_de_goals.size();
    }

private:
    // Inicia el siguiente GOAL en la cola si no hay uno en progreso.
    void procesar_siguiente_goal()
    {
        // Si ya se está procesando un goal o la cola está vacía, no hacer nada
        if (goal_en_progreso_ || cola_de_goals.empty()) {
            return;
        }
        // Obtener el siguiente goal de la cola
        float theta = cola_de_goals.front();
        cola_de_goals.pop();
        goal_en_progreso_ = true; // actualizar que ahora se está procesando un goal
        // Enviar el GOAL al servidor de acción
        enviar_goal(theta);
    }

    // Construye y envía el mensaje de goal al servidor de acción.
    void enviar_goal(float theta)
    {
        // Esperar al servidor antes de enviar
        if (!esperar_al_servidor()) {
            RCLCPP_ERROR(this->get_logger(), "Action server no disponile luego de esperarlo");
            // Si no hay servidor, marcar como no en progreso y probar el siguiente goal
            goal_en_progreso_ = false;
            procesar_siguiente_goal(); // Intentar siguiente goal en la cola
            return;
        }

        // Construir el mensaje de goal
        auto goal_msg = RotateAbsolute::Goal();
        goal_msg.theta = theta;
        RCLCPP_INFO(this->get_logger(), "Enviando goal: theta=%.2f (Tamano Cola: %zu)", theta, obtener_tamano_cola());
        // Opciones para callbacks del envío del goal
        rclcpp_action::Client<RotateAbsolute>::SendGoalOptions options;
        // Asociar callbacks de miembro para las distintas etapas
        options.goal_response_callback = std::bind(&RotateAbsoluteClient::on_goal_response, this,
                                                   std::placeholders::_1);        
        options.feedback_callback = std::bind(&RotateAbsoluteClient::on_feedback, this,
                                              std::placeholders::_1, std::placeholders::_2);
        options.result_callback = std::bind(&RotateAbsoluteClient::on_result, this,
                                            std::placeholders::_1);
        // Enviar asincrónicamente el goal
        this->action_client_->async_send_goal(goal_msg, options);
    }

    // Callback cuando el servidor responde si acepta/rechaza el goal.
    void on_goal_response(GoalHandleRotateAbsolute::SharedPtr goal_handle)
    {
        if (!goal_handle) {
            // goal rechazado por el servidor
            RCLCPP_WARN(this->get_logger(), "Goal fue rechazado por el server");
            goal_en_progreso_ = false;
            // Intentar el siguiente goal si existe
            procesar_siguiente_goal();
        } else {
            // goal aceptado, esperar resultado
            RCLCPP_INFO(this->get_logger(), "Goal aceptado, esperando el resultado");
        }
    }

    // Callback de feedback durante la ejecución del goal.
    void on_feedback(GoalHandleRotateAbsolute::SharedPtr, 
                     const std::shared_ptr<const RotateAbsolute::Feedback> feedback)
    {
        if (feedback) {
            auto grados_restantes = feedback->remaining;
            RCLCPP_INFO(this->get_logger(), "Feedback recibido: [%.2f]", grados_restantes);
        }
    }

    // Callback cuando se recibe el resultado final del goal.
    void on_result(const GoalHandleRotateAbsolute::WrappedResult & wrapped_result)
    {
        if (wrapped_result.code == rclcpp_action::ResultCode::SUCCEEDED) {
            // Resultado exitoso: obtener el delta
            const auto & res = wrapped_result.result->delta;
            RCLCPP_INFO(this->get_logger(), "Result recibido: [%.2f]", res);
        } else {
            // Resultado no exitoso: mostrar código
            RCLCPP_WARN(this->get_logger(), "Result code: %d", 
                       static_cast<int>(wrapped_result.code));
        }
        // Marcar que hemos terminado con este goal
        goal_en_progreso_ = false;
        // Procesar el siguiente goal en la cola, si lo hay
        procesar_siguiente_goal();
    }

    // Espera por el servidor de acciones con reintentos y mensajes informativos.
    bool esperar_al_servidor()
    {
        if (!this->action_client_->wait_for_action_server(std::chrono::seconds(5))) {
            int reintentos = 5;
            // Reintentar esperando 1 segundo varias veces mientras ROS esté activo
            while (rclcpp::ok() && reintentos-- > 0 && 
                   !this->action_client_->wait_for_action_server(std::chrono::seconds(1))) {
                RCLCPP_INFO(this->get_logger(), "Esperando al action server...");
            }
        }
        // Comprueba de forma rápida si el servidor está disponible ahora
        return this->action_client_->wait_for_action_server(std::chrono::seconds(0));
    }

    // Miembros:
    rclcpp_action::Client<RotateAbsolute>::SharedPtr action_client_; // Cliente de acción
    std::queue<float> cola_de_goals; // Cola FIFO de goals (ángulos)
    bool goal_en_progreso_;    // Indicador si hay un goal en ejecución
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto nodo = std::make_shared<RotateAbsoluteClient>();
    // Pone en cola múltiples goals, que se ejecutarán secuencialmente
    nodo->queue_goal(30.0);
    nodo->queue_goal(0.0);
    nodo->queue_goal(-45.0);
    nodo->queue_goal(-135.0);
    nodo->queue_goal(90.0);
    nodo->queue_goal(45.0);
    rclcpp::spin(nodo); 
    rclcpp::shutdown();
    return 0;
}