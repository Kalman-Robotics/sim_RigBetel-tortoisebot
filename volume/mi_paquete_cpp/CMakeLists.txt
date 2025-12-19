cmake_minimum_required(VERSION 3.8)
project(mi_paquete_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED) # para mensajes de twist
find_package(nav_msgs REQUIRED) # para mensajes de odometría
find_package(tf2 REQUIRED) # para transformaciones
find_package(sensor_msgs REQUIRED) # para mensajes del láser

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

# agregamos los nodos y sus dependencias
add_executable(publicador_timer src/publisher_timer.cpp)
ament_target_dependencies(publicador_timer rclcpp geometry_msgs)

add_executable(suscriptor_sensores src/subscriber_sensors.cpp)
ament_target_dependencies(suscriptor_sensores rclcpp nav_msgs tf2 sensor_msgs)

# instalamos los ejecutables
install(TARGETS
  publicador_timer
  suscriptor_sensores
  DESTINATION lib/${PROJECT_NAME}
)

# instala carpetas que se requieran: launch, config, etc
install(DIRECTORY
  launch
  rviz # dado que tenemos archivos en esta carpeta
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
