

OpenCV Examples bearbeiten:
===========================

Umgebungsvariable setzen.
"source sources/poky/oe-init-build-env" oder ". sources/poky/oe-init-build-env" 


lokalisieren des sourcepath mit: <bitbake opencv -c devshell>
Sourcen-> ...\build\tmp\work\aarch64-mx8m-phytec-linux\opencv\3.3+gitAUTOINC+87c27a074d_2a9d1b22ed_a62e20676a_34e4206aef_fccf7cd6a4-r0\git\samples\cpp
verlassen mit <exit>


in den buid path wechseln
compilieren nur opencv teil: bitbake opencv -c compile -f


compilierte binäris -> suchen  <find -name "cpp-example-videocapture_starter">

./build/tmp/sysroots-components/aarch64-mx8m/opencv/usr/share/OpenCV/samples/bin/cpp-example-videocapture_starter

./build/tmp/work/aarch64-mx8m-phytec-linux/opencv/3.3+gitAUTOINC+87c27a074d_2a9d1b22ed_a62e20676a_34e4206aef_fccf7cd6a4-r0/build/bin/cpp-example-videocapture_starter

./build/tmp/work/phyboard_polaris_imx8m_3-phytec-linux/phytec-vision-image/1.0-r0/rootfs/usr/share/OpenCV/samples/bin/cpp-example-videocapture_starter

./build/tmp/work/aarch64-mx8m-phytec-linux/opencv/3.3+gitAUTOINC+87c27a074d_2a9d1b22ed_a62e20676a_34e4206aef_fccf7cd6a4-r0/sysroot-destdir/usr/share/OpenCV/samples/bin/cpp-example-videocapture_starter
