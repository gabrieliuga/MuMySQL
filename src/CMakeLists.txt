# MuMySQLServer is a full featured server for World of Warcraft, supporting
# the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
#
# Copyright (C) 2005-2019  MuMySQLServer project <https://getmangos.eu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Needs to link against mangos_world.lib

if(WIN32)
    if(BUILD_MUSQLSERVER)
        link_directories("${CMAKE_BINARY_DIR}/src/musqlserver/${CMAKE_CFG_INTDIR}")
        include_directories(${CMAKE_SOURCE_DIR}/dep/include)
    endif()
endif()

add_subdirectory(framework)
add_subdirectory(shared)

if(BUILD_MUSQLSERVER)
    add_subdirectory(musqlserver)
	add_subdirectory(connectserver)
endif()

if (BUILD_MUSQLSERVER)
if(WIN32 AND MSVC)
  get_filename_component(MYSQL_LIB_DIR ${MYSQL_LIBRARY} DIRECTORY)
  get_filename_component(MYSQL_LIB_DLL ${MYSQL_LIBRARY} NAME)
  STRING(REPLACE ".lib" ".dll" MYSQL_LIB_DLL ${MYSQL_LIB_DLL})
  install(FILES ${MYSQL_LIB_DIR}/${MYSQL_LIB_DLL} DESTINATION ${BIN_DIR})
  if (OPENSSL_VERSION VERSION_EQUAL "1.1")
    install(FILES ${OPENSSL_ROOT_DIR}/libcrypto-1_1.dll DESTINATION ${BIN_DIR} OPTIONAL)
  else()
    install(FILES ${OPENSSL_ROOT_DIR}/libeay32.dll DESTINATION ${BIN_DIR} OPTIONAL)
  endif()
endif()
endif()
