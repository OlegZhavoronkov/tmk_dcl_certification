# TMK_DCL
General repository for TMK Projects Group. This is INTEGRATION repository.

# Branches

`main` - history of Releases and Tags
`dev` - daily build repository
`TMKDCL-xxx` - developer branches by issues in Jira for merge by pull-request strategy to dev-branch

# Build and run system in docker

Prepare your docker image //TODO: or use it from the project repository 

//TODO: write how-to

Prepare and run data sources and server (process pipeline adn tcp command server) as docker services

//TODO:

Prepare pipline configuration file
Run the system manager (tcp client //TODO: or send http requessts). You can use `netcat` utility for this goals.

Start the system using commands by command API
//TODO: command specification

You can attach to process for debug //TODO: write how-to
//TODO: attach isn't testing now

Obtain the results (logs) of process using `docker log ???` 
//TODO: find the better solution then docker log

# Build and run on the host system

!ATTENTION! You CAN build and run this solution in SOS cases directly. Enviroment is prepared for this issue. But it's strongly NOT RECOMMENDED.

Enviroment:
```
sudo apt install //TODO: lib
```
Build:
```
git clone  --recurce-submodule  https://github.com/tmk-org/TMK_DCL.git
cd TMK_DCL
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```
Run from build-directory
```
cd dclcomponents
```
Start data source
```
  TEST_SOURCE_PATH="/mnt/testdb/pencil_frames_5" bin/tmk-camera 9000
```
UPD: for usage more then 1 source ports should be 9000 + 10 * i, where i is source number
Start server
```
  GLOG_minloglevel=2 bin/tmk-server 4444
```
PS: (Not tested) you can start tmk-camera and tmk-server as daemon with 
//TODO: check the work is correct
```
bin/tmk-server 4444 1
```
Start command tcp client
```
bin/tmk-manager localhost 4444
```
(or use `netcat` utility for this goals as `netcat localhost 4444`)

Test system:
1) Send `stop` command to server by tcp. After some waitting the system should be stopped without any errors.
2) Send `config:default` command for start pipeline with empty middle or `config:pg` command for start with connection to the database
3) You can send `config:demo` for string filter pipeline idea demonstration
4) (Feature: not implemented) Send `init` command for start empty pipeline, other modules could be attached in pipeline by commands
5) (Not tested) Send `config:path_to_json` command

//TODO: init command by IP has paramerer - configuration .json-file for configure pipeline -- in progress

//TODO: 

Check that all modules are started (by default: `tmk-server`, `tmk-first`, `tmk-middle`, `tmk-last`)
```
ps aux | grep tmk
```
3) Send `stop` command. Pipeline should be stopped correctly. Check that all modules are finished.

Run sender to winnum (database shoould be available)
```
cd ../winnum
python3 sent_to_winnum.py
```

# Tips and tricks

Configuration is hard-code now =(

Pipeline configuration: dclprocessor/src/pipeline_demo.c, функция create_default_configuratuion. Важно: количество модулей, их порядок, строка старта. First и last фикстированы. Учесть, что количество модулей = количество объектов + 1 в памяти одновременно.

Capture configuration: dclprocessor/src/collector_description.c, функция read_collector_description. Важно: количество источников, выделяемая память под объекты (в случае, если не хватит - будет падение)

Number frames in object: dclprocessor/include/dclprocessor/source.h, #define OBJECT_DETECTION_FRAME_LIMIT 50 (в минимальном прототипе 100, но хочется укладываться в сумме в 16Гб памяти)

Fake tcp-camera: dclprocessor/include/dclprocessor/cvcamera.h, размер количество кадров в датасете, который "стримим". По умолчанию dataset "pencil_frames_5", первые 10 кадров по циклу


