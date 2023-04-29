<!DOCTYPE html>
<html lang="en">
<head>
    <title>Send JSON to C++ Client</title>
</head>
<body>
<h1>Send JSON to C++ Client</h1>
<form action="send_json_action.php" method="post">
    <label for="json">Enter JSON data:</label><br>
    <textarea name="json" id="json" rows="4" cols="50">
{
    "name": "John Doe",
    "age": 30
}
        </textarea><br>
    <input type="submit" value="Send">
</form>
</body>
</html>
