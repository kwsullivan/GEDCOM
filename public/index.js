// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    $('#login-form').modal('show');
});

$(document).ready(function() {


    $('#login-form').submit(function(e) {
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/database-login',
            data: {
                database: $('#databaseName').val(),
                username: $('#userName').val(),
                password: $('#password').val(),
            },
            success: function(data) {
                console.log(data);
                $("#login-form").modal("hide");
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/filelog',   //The server endpoint we are connecting to
        success: function (data) {

            for(i in data.files) {

                

                //<option value="text1">text1</option>
                $('#status-box').append("Successfully parsed: " + data.files[i] + "<br>");
                if(i == 0) {
                    $('#file-dropdown').append("<option selected id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
                }
                else {
                    $('#file-dropdown').append("<option id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
                }

               
               $('#indiv-dropdown').append("<option id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
               $('#desc-dropdown').append("<option id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
               $('#ancs-dropdown').append("<option id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
               $('#modal-file-dropdown').append("<option selected id=" + data.files[i] + ">" + data.files[i] + "</option>" + 
                    "<br>");
                    
                $('#log-body').append("<tr>");
                $('#log-body').append("<td><a href=uploads/" + data.files[i] + ">"+ data.files[i] + "</a></td>");

                $('#log-body').append("<td>" + data.info[i].source + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].gedc + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].encoding + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].submitter + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].address + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].indivs + "</a></td>");
                $('#log-body').append("<td>" + data.info[i].fams + "</a></td>");                
                $('#log-body').append("</tr>");

            }

            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */

            //We write the object to the console to show that the request was successful
            
            //console.log(data);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
        $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/viewpanel',
        success: function (data) {

        },
        fail: function(error) {
            console.log(error);
        }
    });
    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    
    $('#create-form').submit(function(e) {
        e.preventDefault();
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/createGEDCOM',
            data: {
                filename: $('#filename').val(),
                submitter: $('#submitter').val(),
                address: $('#address').val(),
            },
            success: function(data) {
                console.log(data);
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    $('#file-dropdown').click(function() {
        var mytextbox = document.getElementById('selected-file');
            mytextbox.value = this.value;
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/viewpanel',
            data: {
                file: this.value
            },
            success: function(data) {
                $('#view-body').html("");
                for(i in data) {
                    $('#view-body').append("<tr>");
                    $('#view-body').append("<td>" + data[i].givenName + "</a></td>");
                    $('#view-body').append("<td>" + data[i].surname + "</a></td>");
                    $('#view-body').append("<td>" + data[i].sex + "</a></td>");
                    $('#view-body').append("<td>" + data[i].famSize + "</a></td>");
                    $('#view-body').append("</tr>");
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
    $('#indiv-dropdown').click(function() {
        var mytextbox = document.getElementById('indiv-file');
            mytextbox.value = this.value;
    });
    $('#add-indiv').submit(function() {

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/addindiv',
            data: {
                file: $('#indiv-file').val(),
                first: $('#indivFirst').val(),
                last: $('#indivLast').val(),
            },
            success: function(data) {
                console.log(data);
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
    $('#desc-dropdown').click(function() {
        var mytextbox = document.getElementById('desc-file');
            mytextbox.value = this.value;
    });
    $('#desc-form').submit(function() {

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/addindiv',
            data: {
                file: $('#indiv-file').val(),
                first: $('#indivFirst').val(),
                last: $('#indivLast').val(),
            },
            success: function(data) {
                console.log(data);
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
    $('#ancs-dropdown').click(function() {
        var mytextbox = document.getElementById('ancs-file');
            mytextbox.value = this.value;
    });

    $('#add-indiv').submit(function() {

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/addindiv',
            data: {
                file: $('#indiv-file').val(),
                first: $('#indivFirst').val(),
                last: $('#indivLast').val(),
            },
            success: function(data) {
                console.log(data);
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
/******************************************
 * ASSIGNMENT 4 FUNCTIONALITY
 ******************************************/

     $('#modal-file-dropdown').click(function() {
        var mytextbox = document.getElementById('file-display');
            mytextbox.value = this.value;
    });

    $('#from-file-sort').click(function() {
        $('#from-file-modal').modal('show');
    });

    $('#all-indiv-last').click(function() {
        $('#all-indivs-modal').modal('show');
    });

    $('#option-3').click(function() {
        $('#modal-3').modal('show');
    });

    $('#option-4').click(function() {
        $('#modal-4').modal('show');
    });

    $('#option-5').click(function() {
        $('#modal-5').modal('show');
    });


});

$(document).load(function() {

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/example',
        success: function(data) {
            console.log(data);

        },
        fail: function(error) {
            console.log(error);
        }
    });

});
