%%{
    machine State;

    import "events.h";

Proxy := (
        start: ( 
           CONNECT @proxy_deliver -> Sending |
           FAILED @proxy_failed -> Closing
        ),

        Proxying: (
            HTTP_REQ @proxy_deliver -> Sending |
            PROXY @proxy_exit_routing |
            HANDLER @proxy_exit_routing |
            DIRECTORY @proxy_exit_routing |
            REMOTE_CLOSE @proxy_close -> Closing
        ),

        Sending: (
            REQ_SENT @proxy_parse -> Proxying |
            REMOTE_CLOSE @proxy_close -> Closing
        ),

        Closing: (
            CLOSE @proxy_exit_idle
        )

     )  <err(error);


Connection = (
        start: ( OPEN @open -> Accepting ),

        Accepting: ( ACCEPT @parse -> Idle ),

        Idle: (
            REQ_RECV @identify_request HTTP_REQ @route_request -> HTTPRouting |
            REQ_RECV @identify_request MSG_REQ @route_request -> MSGRouting |
            REQ_RECV @identify_request SOCKET_REQ @send_socket_response -> Responding |
            CLOSE @close -> final
        ),

        MSGRouting: (
            HANDLER @msg_to_handler -> Queueing |
            DIRECTORY @msg_to_directory -> Responding 
        ),

        HTTPRouting: (
            HANDLER @http_to_handler -> Queueing |
            PROXY @http_to_proxy  |
            DIRECTORY @http_to_directory -> Responding |
            CLOSE @close -> final
        ),

        Queueing: ( REQ_SENT @parse -> Idle ),

        Responding: (
            RESP_SENT @parse -> Idle |
            CLOSE @close -> final
        )

        ) %eof(finish) <err(error);

main := (Connection)*;

}%%