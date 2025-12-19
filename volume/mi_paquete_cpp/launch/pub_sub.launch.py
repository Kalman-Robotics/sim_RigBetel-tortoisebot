# Importa la clase LaunchDescription para describir el lanzamiento
from launch import LaunchDescription
# Importa la acción Node para lanzar nodos ROS2
from launch_ros.actions import Node

def generate_launch_description():
    # Nodo que ejecuta el publicador con timer
    nodo_publicador_timer = Node(
        package='mi_paquete_cpp',
        executable='publicador_timer',
        name='publicador_timer',
        output='screen'
    )
    
    # Nodo que ejecuta el suscriptor de sensores
    nodo_suscriptor_sensores = Node(
        package='mi_paquete_cpp',
        executable='suscriptor_sensores',
        name='suscriptor_sensores',
        output='screen'
    )
    
    # Retorna la descripción del lanzamiento con ambos nodos
    return LaunchDescription([
        nodo_publicador_timer,
        nodo_suscriptor_sensores
    ])