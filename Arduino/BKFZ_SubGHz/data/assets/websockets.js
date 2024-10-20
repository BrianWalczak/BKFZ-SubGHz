$(document).ready(function () {
    window.ws = new WebSocket(`ws://${window.location.host}/ws`);

    // Handle websocket message with custom callback
    window.ws.onmessage = function(event) {
        const dataParsed = JSON.parse(event.data);

        if(dataParsed.url == document.location.pathname) {
            handleWs(dataParsed.data);
        }
    };

    // Function to send a message through the websocket
    window.sendMessage = function(data) {
        const body = {
            url: document.location.pathname,
            data
        };

        ws.send(JSON.stringify(body));
    }

    $(window).on('beforeunload', function() {
        window.ws.close(); // Make sure to close the websocket once done
    });
});