void MCP_begin() {
  SPI.begin();

  Serial.println("Configure ACAN2515");
  Serial.println("Configure help");

  ACAN2515Settings settings(QUARTZ_FREQUENCY, 125UL * 1000UL); // CAN bit rate 125 kb/s

  const uint16_t errorCode = can.begin (settings, NULL);

  if (errorCode == 0) {
    Serial.print("Bit Rate prescaler: ");
    Serial.println(settings.mBitRatePrescaler);
    Serial.print("Propagation Segment: ");
    Serial.println(settings.mPropagationSegment);
    Serial.print("Phase segment 1: ");
    Serial.println(settings.mPhaseSegment1);
    Serial.print("Phase segment 2: ");
    Serial.println(settings.mPhaseSegment2);
    Serial.print("SJW: ");
    Serial.println(settings.mSJW);
    Serial.print("Triple Sampling: ") ;
    Serial.println(settings.mTripleSampling ? "yes" : "no") ;
    Serial.print("Actual bit rate: ") ;
    Serial.print(settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print("Exact bit rate ? ") ;
    Serial.println(settings.exactBitRate () ? "yes" : "no") ;
    Serial.print("Sample point: ") ;
    Serial.print(settings.samplePointFromBitStart ()) ;
    Serial.println("%") ;
  } else {
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

void CAN_bus_send(byte data_0, byte data_1, byte data_2, byte data_3, byte data_4, byte data_5, byte data_6, byte data_7) {
  can.poll();
  CANMessage frame;
  frame.ext = true;
  frame.id = 0x01;
  frame.len = 8;
  frame.data [0] = data_0 ;
  frame.data [1] = data_1 ;
  frame.data [2] = data_2;
  frame.data [3] = data_3 ;
  frame.data [4] = data_4 ;
  frame.data [5] = data_5 ;
  frame.data [6] = data_6 ;
  frame.data [7] = data_7 ;

  if (can.tryToSend (frame)) {
    Serial.println ("Sent") ;
  } else {
    Serial.println ("Send failure") ;
  }
}
