#!/bin/sh

###############################################################################
# Copyright (c) 2017, PulseRain Technology LLC 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (LGPL) as 
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE.  
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

FP51_version=1.3.2
M10_compiler_version=1.2.1
M10_upload_version=1.1.0


FP51_pkg_name="FP51_"
FP51_pkg_name+=$FP51_version
FP51_pkg_name+=".tar.gz"
#echo "FP51_pkg_name = $FP51_pkg_name"
cd ./FP51;tar zcf ../package/$FP51_pkg_name .;cd ..
sha256_FP51=$(sha256sum ./package/$FP51_pkg_name | awk '{print $1}')
size_FP51=$(stat -c %s ./package/$FP51_pkg_name)

M10_compiler_name="M10_compiler_"
M10_compiler_name+=$M10_compiler_version
M10_compiler_name+=".tar.gz"
#echo "M10_compiler_name = $M10_compiler_name"
cd ./M10_compiler;tar zcf ../package/$M10_compiler_name .;cd ..
sha256_M10_compiler=$(sha256sum ./package/$M10_compiler_name | awk '{print $1}')
size_M10_compiler=$(stat -c %s ./package/$M10_compiler_name)


M10_upload_name="M10_upload_"
M10_upload_name+=$M10_upload_version
M10_upload_name+=".tar.gz"
#echo "M10_upload_name = $M10_upload_name"
cd ./M10_upload;tar zcf ../package/$M10_upload_name .;cd ..
sha256_M10_upload=$(sha256sum ./package/$M10_upload_name | awk '{print $1}')
size_M10_upload=$(stat -c %s ./package/$M10_upload_name)

#echo "=============================================================================="
#echo "    package_M10_index.json"
#echo "=============================================================================="
echo '{'
echo '   "packages": ['
echo '     {'
echo '      "name": "PulseRain_M10",'
echo '      "maintainer": "PulseRain Technology, LLC",'
echo '      "email": "info@pulserain.com",'
echo '      "help": {'
echo '        "online": "http://M10.pulserain.com"'
echo '      },'
echo '      "websiteURL": "http://M10.pulserain.com",'
echo '      "platforms": ['
echo '       {'
echo '          "name": "PulseRain_M10",'
echo '          "architecture": "FP51",'
echo "          \"version\": \"$FP51_version\","
echo '          "category": "FP51",'
echo "          \"url\": \"https://github.com/PulseRain/Arduino_M10_IDE/raw/master/package/$FP51_pkg_name\","
echo "          \"archiveFileName\": \"$FP51_pkg_name\","
echo "          \"checksum\": \"SHA-256:$sha256_FP51\","
echo "          \"size\": \"$size_FP51\","
echo '          "help": {'
echo '            "online": "http://M10.pulserain.com"'
echo '          },'
echo '          "boards": ['
echo '            {"name": "PulseRain_M10"}'
echo '          ],'
echo '          "toolsDependencies": ['
echo '            {'
echo '              "packager": "PulseRain_M10",'
echo "              \"version\": \"$M10_upload_version\","
echo '              "name": "M10_upload"'
echo '            },'
echo '            {'
echo '              "packager": "PulseRain_M10",'
echo "              \"version\": \"$M10_compiler_version\","
echo '              "name": "M10_compiler"'
echo '            }'
echo '          ]'
echo '        }'       
echo '      ],'
echo '      "tools": ['
echo '        {'
echo "          \"version\": \"$M10_upload_version\","
echo '          "name": "M10_upload",'
echo '          "systems": ['
echo '            {'
echo '              "host": "i686-mingw32",'
echo "              \"url\": \"https://github.com/PulseRain/Arduino_M10_IDE/raw/master/package/$M10_upload_name\","
echo "              \"archiveFileName\": \"$M10_upload_name\","
echo "              \"checksum\": \"SHA-256:$sha256_M10_upload\","
echo "              \"size\": \"$size_M10_upload\""
echo '            }'
echo '          ]'
echo '        },'
echo '        {'
echo "          \"version\": \"$M10_compiler_version\","
echo '          "name": "M10_compiler",'
echo '          "systems": ['
echo '            {'
echo '              "host": "i686-mingw32",'
echo "              \"url\": \"https://github.com/PulseRain/Arduino_M10_IDE/raw/master/package/$M10_compiler_name\","
echo "              \"archiveFileName\": \"$M10_compiler_name\","
echo "              \"checksum\": \"SHA-256:$sha256_M10_compiler\","
echo "              \"size\": \"$size_M10_compiler\""
echo '            }'
echo '          ]'        
echo '        }'
echo '      ]'
echo '    }'
echo '  ]'
echo '}'
