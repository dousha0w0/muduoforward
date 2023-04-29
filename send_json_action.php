<?php
function send_json($json_data, $server_address, $server_port) {
    $client_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    if ($client_socket === false) {
        die("socket_create() failed: " . socket_strerror(socket_last_error()) . "\n");
    }

    $client_address = "localhost";
    $result = socket_connect($client_socket, $client_address, $server_port);
    if ($result === false) {
        die("socket_connect() failed: " . socket_strerror(socket_last_error($client_socket)) . "\n");
    }

    $client_socket_fd = (int) $client_socket;

    $cmd = "./json_forward_client " . escapeshellarg($server_address) . " " . escapeshellarg($server_port) . " " . escapeshellarg($client_socket_fd);
    $descriptorspec = array(
        0 => array("pipe", "r"),
        1 => array("pipe", "w"),
        2 => array("pipe", "w")
    );

    $process = proc_open($cmd, $descriptorspec, $pipes);

    if (is_resource($process)) {
        fwrite($pipes[0], $json_data);
        fclose($pipes[0]);

        $response = stream_get_contents($pipes[1]);
        fclose($pipes[1]);

        $error_output = stream_get_contents($pipes[2]);
        fclose($pipes[2]);

        $return_value = proc_close($process);

        if ($return_value == 0) {
            echo "Data sent successfully.<br>";
            echo "Response: " . $response . "<br>";
        } else {
            echo "Failed to send data. Error: " . $error_output . "<br>";
        }
    } else {
        echo "Failed to start the C++ client.<br>";
    }

    socket_close($client_socket);
}

$json_data = $_POST['json_data'];
$server_address = "localhost";
$server_port = 8888;

send_json($json_data, $server_address, $server_port);
?>
