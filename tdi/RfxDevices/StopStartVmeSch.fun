public fun StopStartVmeSch(in _node)
{

   if(_node == "EDA1" || _node == "ALL")
   {	
      MdsConnect("150.178.34.30");
      write(*,"Stop e start dello scheduler su EDA1"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R1","150.178.34.130", val(2008), val(1))');
      MdsDisconnect();
   }

   if(_node == "DEQU" || _node == "ALL")
   {	

      MdsConnect("150.178.34.45");
      write(*,"Stop e start dello scheduler su DEQU"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(1);
      MdsValue('VmeStateMachine->engdaStart("R2","150.178.34.130", val(2014), val(1))');
      MdsDisconnect();

   }


   if(_node == "EDA3" || _node == "ALL")
   {	
      MdsConnect("150.178.34.31");
      write(*,"Stop e start dello scheduler su EDA3"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R3","150.178.34.130", val(2009), val(1))');
      MdsDisconnect();
   }


   if(_node == "DFLU" || _node == "ALL")
   {	

      MdsConnect("150.178.34.46");
      write(*,"Stop e start dello scheduler su DFLU"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(1);
      MdsValue('VmeStateMachine->engdaStart("R4","150.178.34.130", val(2015), val(1))');
      MdsDisconnect();

   }

   if(_node == "MHD_BR" || _node == "ALL")
   {	
      MdsConnect("150.178.34.27");
      write(*,"Stop e start dello scheduler su MHD_BR"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R5","150.178.34.130", val(2012), val(1))');
      MdsDisconnect();
   }


   if(_node == "MHD_AC" || _node == "ALL")
   {	
      MdsConnect("150.178.34.29");
      write(*,"Stop e start dello scheduler su MHD_AC"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R6","150.178.34.130", val(2010), val(1))');
      MdsDisconnect();
   }

   if(_node == "MHD_BC" || _node == "ALL")
   {	
      MdsConnect("150.178.34.28");
      write(*,"Stop e start dello scheduler su MHD_BC"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R7","150.178.34.130", val(2011), val(1))');
      MdsDisconnect();
   }

   if(_node == "MHD_I" || _node == "ALL")
   {	
      MdsConnect("150.178.34.15");
      write(*,"Stop e start dello scheduler su MHDI"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R8","150.178.34.130", val(2016), val(1))');
      MdsDisconnect();
   }

   if(_node == "EDAV" || _node == "ALL")
   {	
      MdsConnect("192.168.34.9");
      write(*,"Stop e start dello scheduler su EDAV"); 
      MdsValue("VmeStateMachine->engdaStop()");
      wait(2);
      MdsValue('VmeStateMachine->engdaStart("R9","150.178.34.130", val(2017), val(1))');
      MdsDisconnect();
   }

   return (1);
}
