# tortoisebot

Esta es una simulación dockerizada que permite al usuario tener un entorno listo para usar.

![robot](pictures/results.png)

- [tortoisebot](#tortoisebot)
  - [Pasos:](#pasos)
  - [Referencias:](#referencias)


## Pasos:

> [!NOTE] 
> Todos los siguientes pasos pueden ser obviados si se utiliza **DevContainer** en VSCode.

**1. Dirígete al directorio donde se encuentra el archivo docker-compose y ejecuta el siguiente comando:**
```
cd <PATH_TO_REPO>/sim_tortoisebot
sudo docker-compose -f docker-compose-ros2.yml up --build
```
Si lo haces por primera vez, tomará un tiempo descargar todos los recursos necesarios y construir el contenedor.

> 📢 Cuando el contenedor termine su configuración, verás el siguiente mensaje:
> ```
> tortoisebot_development_container | ----------SETUP COMPLETE----------
> ```

**2. Cuando no lo haces por primera vez, es posible que obtengas un error. Soluciona el problema con el siguiente comando:**

<details closed>
<summary>Ejemplo de error</summary>

```
ERROR: for tortoisebot_development_container  'ContainerConfig'

ERROR: for development_server  'ContainerConfig'
Traceback (most recent call last):
  File "docker-compose", line 3, in <module>
  File "compose/cli/main.py", line 81, in main
  File "compose/cli/main.py", line 203, in perform_command
  File "compose/metrics/decorator.py", line 18, in wrapper
  File "compose/cli/main.py", line 1186, in up
  File "compose/cli/main.py", line 1182, in up
  File "compose/project.py", line 702, in up
  File "compose/parallel.py", line 108, in parallel_execute
  File "compose/parallel.py", line 206, in producer
  File "compose/project.py", line 688, in do
  File "compose/service.py", line 581, in execute_convergence_plan
  File "compose/service.py", line 503, in _execute_convergence_recreate
  File "compose/parallel.py", line 108, in parallel_execute
  File "compose/parallel.py", line 206, in producer
  File "compose/service.py", line 496, in recreate
  File "compose/service.py", line 615, in recreate_container
  File "compose/service.py", line 334, in create_container
  File "compose/service.py", line 922, in _get_container_create_options
  File "compose/service.py", line 962, in _build_container_volume_options
  File "compose/service.py", line 1549, in merge_volume_bindings
  File "compose/service.py", line 1579, in get_container_data_volumes
KeyError: 'ContainerConfig'
[34211] Failed to execute script docker-compose
```
</details>

```
docker container prune -f # remove all containers
```

**3. Abre una nueva terminal y ejecuta el siguiente comando para acceder al contenedor:**
```
docker exec -it tortoisebot_development_container /bin/bash
```
Luego puedes iniciar la simulación
```
ros2 launch tortoisebot_bringup bringup.launch.py use_sim_time:=True
ros2 launch tortoisebot_bringup bringup_vacio.launch.py use_sim_time:=True
ros2 launch tortoisebot_bringup bringup_laberinto.launch.py use_sim_time:=True
```

## Referencias:
- [repo](https://github.com/rigbetellabs/tortoisebot)
- [install dependency](https://github.com/YDLIDAR/YDLidar-SDK/blob/master/doc/howto/how_to_build_and_install.md)
- [usage](https://github.com/rigbetellabs/tortoisebot/wiki/5.-ROS2)