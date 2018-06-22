/** @file

 A brief file description

 @section license License

 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/****************************************************************************

 IncomingConnectHandler.h

 Detects whether the incoming connection is raw SSL or a CONNECT request
 If a CONNECT request then it reads in the incoming CONNECT request and
 sends back a response

 ****************************************************************************/

#if !defined(_IncomingConnectHandler_h_)
#define _IncomingConnectHandler_h_

#include "ConnectHandler.h"

class IncomingConnectHandler: public ConnectHandler
{
public:
  IncomingConnectHandler(SSLNetVConnection *inSslNetVConn);

  int doWork() override;

private:
  int detectConnect();
  int parseIncomingConnect();
  int sendConnectResponse();

  bool checkedForConnect = false;
  bool connectReceived = false;
};

#endif /* _IncomingConnectHandler_h_ */
