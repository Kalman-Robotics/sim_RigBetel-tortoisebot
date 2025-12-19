cmake_minimum_required(VERSION 3.8)
project(mis_componentes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED) # 1. necesario para componentes
find_package(std_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 2. utilizamos add_library para crear las librerias de los componentes
# 3. linkeamos las dependencias necesarias
# agregamos el 1er componente 
add_library(publicador_timer_component SHARED src/publisher_timer.cpp)
ament_target_dependencies(publicador_timer_component rclcpp rclcpp_components geometry_msgs)
target_link_libraries(publicador_timer_component rclcpp_components::component)
# agregamos el 2do componente 
add_library(suscriptor_sensores_component SHARED src/subscriber_sensors.cpp)
ament_target_dependencies(suscriptor_sensores_component rclcpp rclcpp_components nav_msgs tf2 sensor_msgs)
target_link_libraries(suscriptor_sensores_component rclcpp_components::component)

# 4. declaramos los componentes (plugins)
# registramos 1er componente(plugin con formato name::Clase)
rclcpp_components_register_node(
  publicador_timer_component
  PLUGIN "mis_componentes::RobotMotionPublisher"
  EXECUTABLE publicador_timer
)
# registramos 2do componente(plugin con formato name::Clase)
rclcpp_components_register_node(
  suscriptor_sensores_component
  PLUGIN "mis_componentes::RobotSensorsSubscriber"
  EXECUTABLE suscriptor_sensores
)

# 5. instalamos la versiones de las bibliotecas
# instalamos la version compartida del 1er componente
ament_export_targets(export_publicador_timer_component)
install(TARGETS publicador_timer_component
        EXPORT export_publicador_timer_component
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
# instalamos la version compartida del 2do componente
ament_export_targets(export_suscriptor_sensores_component)
install(TARGETS suscriptor_sensores_component
        EXPORT export_suscriptor_sensores_component
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

# instala carpetas que se requieran: launch, etc
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
