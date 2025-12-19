# Importa la clase LaunchDescription para describir el lanzamiento
from launch import LaunchDescription
# Importa la acción ComposableNodeContainer para contener nodos componibles
from launch_ros.actions import ComposableNodeContainer
# Importa la acción ComposableNode para lanzar nodos componibles
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    # Nodos componibles definidos por separado para mayor legibilidad
    componente1 = ComposableNode(
        package='mis_componentes',
        plugin='mis_componentes::RobotMotionPublisher',
        name='robot_motion_publisher',
    )

    componente2 = ComposableNode(
        package='mis_componentes',
        plugin='mis_componentes::RobotSensorsSubscriber',
        name='robot_sensors_subscriber',
    )

    # Contenedor que agrupa los nodos componibles
    contenedor = ComposableNodeContainer(
        name='mi_contenedor',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            componente1,
            componente2,
        ],
        output='screen'
    )
    # Retorna la descripción del lanzamiento con ambos nodos
    return LaunchDescription([
        contenedor,
    ])