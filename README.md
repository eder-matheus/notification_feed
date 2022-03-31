# notification_feed

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/180bacb0bd03452d9c89a705eebbd8f5)](https://app.codacy.com/gh/eder-matheus/notification_feed?utm_source=github.com&utm_medium=referral&utm_content=eder-matheus/notification_feed&utm_campaign=Badge_Grade_Settings)

Repository with the final assignment of the Operational Systems II discipline of the Computer Science course (UFRGS).

## Getting started
### Requirements
  * CMake 3.2.2 (or latest)
  * C++ 11 (or latest)

*NOTE*: The following scripts should be run on the root directory of the repository (i.e.: on `/path/to/notification_feed/`)
### How to install
The `build.sh` file has three available options, desribed below.
  * Run `source build.sh` to compile both server and client applications. The build files will be availale on `/path/to/notification_feed/build`
  * Run `source build.sh clear` to delete the build files. The directory `/path/to/notification_feed/build` will be fully deleted
  * Run `source build.sh nuke` to delete everything from the build and environment files/directories (described in the next section).

### Running the server and client applications
The notification_feed system has two main entities: the server, and the client applications. Next, is  described the commands to start the server and to start client applications.
*NOTE*: The following commands will only work if you have builded the project (i.e., executed the `build.sh` script).
  * To start the server, run `source start_server.sh`. The script will create the directory `/path/to/notification_feed/server_env`, where a symlink to the server binary is create and the database file created by the server is saved.
      * You can end the server application with the `Ctrl+C` command. This will have you back to the root directory of the repository.
  * To start a client application, run `source start_client.sh @username server gate`. The script will create the directory `/path/to/notification_feed/client_env`, where a symlink to the client binary is created. Multiple clients can share the same binary to start its applications.
      * Alternatively, you can run `source start_client.sh` without the three parameters. This will lead you to the `/path/to/notification_feed/client_env` directory, where you can manually call the `client` binary with the correct parameters.
      * You can end the client application with `Ctrl+D` or `Ctrl+C` commands. Both of them will have you back to the root directory of the repository.

Note that while you do not *nuke* the application, all the data stored in the server's database file will be imported at the server initialization of each new run. This means that the users relation (who follow who) is persistent between subsequent server runs.