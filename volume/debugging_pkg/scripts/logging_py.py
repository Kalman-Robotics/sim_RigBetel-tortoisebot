#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

class EjemploLogging(Node):
    def __init__(self):
        super().__init__('ejemplo_logging_py')

        # Establecer el nivel del logger a DEBUG
        self.get_logger().set_level(rclpy.logging.LoggingSeverity.DEBUG)

        self.get_logger().info('El nodo ha sido inicializado.')

        # Ejemplo de log en diferentes niveles
        self.mostrar_mensajes()

    def mostrar_mensajes(self):
        self.get_logger().debug('Este es un mensaje DEBUG.')
        self.get_logger().info('Este es un mensaje INFO.')
        self.get_logger().warn('Este es un mensaje WARN.')
        self.get_logger().error('Este es un mensaje ERROR.')
        self.get_logger().fatal('Este es un mensaje FATAL.')

def main(args=None):
    # Inicializar el sistema ROS 2
    rclpy.init(args=args)

    # Crear el nodo y ejecutarlo
    nodo = EjemploLogging()
    rclpy.spin(nodo)

    # Apagar el sistema ROS 2
    rclpy.shutdown()

if __name__ == '__main__':
    main()