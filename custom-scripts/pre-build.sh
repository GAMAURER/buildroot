  #!/bin/sh
  
  cp $BASE_DIR/../custom-scripts/S41network-config $BASE_DIR/target/etc/init.d
  chmod +x $BASE_DIR/target/etc/init.d/S41network-config
  
  cp $BASE_DIR/../custom-scripts/S42server $BASE_DIR/target/etc/init.d
  chmod +x $BASE_DIR/target/etc/init.d/S42server
