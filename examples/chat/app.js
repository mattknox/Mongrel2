
function escapeHTML( html ) {
    var trans = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#x27;'
    };
    return (html + '').replace(/[&<>\"\']/g, function(c) { return trans[c]; });
}

function setUsers(users) {
    // ignored for now
}

function addUser(user) {
    // ignored for now
}

function removeUser(user) {
    // ignored for now
}

function addMessage(text) {
    var div = document.createElement("div");
    div.innerHTML = text;
    var cl = document.getElementById("chatlog");
    cl.appendChild(div);
    div.scrollIntoView();
}

state = new FSM({
    start: function(fsm, event) {
        // var user = prompt('What is your nick?', 'lamenick');
        fsm.messages = {};
        fsm.trans('connecting');
        Chat.init('zed', fsm);
    },

    connecting: {
        CONNECT: function(fsm, event) {
            addMessage('<em>Connected to Mongrel2.org chat demo.</em>');
            Chat.socket.send({'type': 'join', 'user': Chat.user});
            fsm.trans('connected');
        },
    },


    connected: {
        MSG: function(fsm, event) {
            addMessage('<b>[ ' + escapeHTML(event.user) + ' ]</b> ' + escapeHTML(event.msg));
        },

        JOIN: function(fsm, event) {
            if(event.user) {
                addMessage('<em><b>' + escapeHTML(event.user) + '</b> joined chat.</em>');
                addUser(event.user);
            }
        },

        LEAVE: function(fsm, event) {
            if(event.user) {
                addMessage('<em><b>' + escapeHTML(event.user) + '</b> left chat.</em>');
                removeUser(event.user);
            }
        },

        USERLIST: function(fsm, event) {
            setUsers(event.users);
        },

        CLOSE: function(fsm, event) {
            addMessage('<em>Well, looks like Mongrel2 crashed or your internet did.</em>');
            fsm.trans('connecting');
        },

        SEND: function(fsm, event) {
            var input = document.getElementById("msginput");
            Chat.send(input.value);
            input.value = '';
        }
    }

});

