from launch import LaunchDescription # para describir el lanzamiento
from launch_ros.actions import Node # para lanzar nodos ROS2
from launch.actions import (
    DeclareLaunchArgument, # para declarar parámetros
    LogInfo # para imprimir en la consola
    )
from launch.substitutions import (
    LaunchConfiguration, # para emplear parámetros
    PythonExpression # para evaluar expresiones Python
    )

def generate_launch_description():
    # sustituciones para recibir los parámetros declarados
    param_string = LaunchConfiguration('param_string')
    param_bool = LaunchConfiguration('param_bool')
    param_int = LaunchConfiguration('param_int')
    param_double = LaunchConfiguration('param_double')
    param_rviz_config = LaunchConfiguration('rviz_config')

    # log de mensajes realizando operaciones con los parámetros
    log_result_string = LogInfo(
        msg=['Saludos para: ', PythonExpression(['"', param_string, '".upper()']), '*']
    )
    log_result_bool = LogInfo(
        msg=[
            'El valor booleano negado es: ',
            PythonExpression(['not ', param_bool])
        ]
    )
    log_result_int = LogInfo(
        msg=[
            'El valor entero multiplicado por 10 es: ',
            PythonExpression([param_int, ' * 10'])
        ]
    )
    log_result_double = LogInfo(
        msg=[
            'El valor double dividido por 2 es: ',
            PythonExpression([param_double, ' / 2.0'])
        ]
    )
    
    # declaración de parámetros con valores por defecto
    declarar_param_string = DeclareLaunchArgument(
        'param_string',
        default_value='valor_por_defecto',
        description='Un parámetro de tipo string'
    )
    declarar_param_bool = DeclareLaunchArgument(
        'param_bool',
        default_value='true',
        description='Un parámetro de tipo booleano'
    )
    declarar_param_int = DeclareLaunchArgument(
        'param_int',
        default_value='42',
        description='Un parámetro de tipo entero'
    )
    declarar_param_double = DeclareLaunchArgument(
        'param_double',
        default_value='3.14',
        description='Un parámetro de tipo double'
    )
    declarar_param_rviz_config = DeclareLaunchArgument(
        'rviz_config',
        default_value='/ros2_ws/src/mi_paquete_cpp/rviz/empty.rviz',
        description='Ruta al archivo de configuración de RViz2'
    )
    
    # nodo de RViz2
    rviz_nodo = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', param_rviz_config],
    )
    
    # Retorna la descripción del lanzamiento con ambos nodos
    return LaunchDescription([
        # declarar parámetros
        declarar_param_string,
        declarar_param_bool,
        declarar_param_int,
        declarar_param_double,
        declarar_param_rviz_config,
        # log informativo
        log_result_string,
        log_result_bool,
        log_result_int,
        log_result_double,
        # ejecutar nodos
        rviz_nodo,
    ])