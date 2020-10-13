var DEBUG = true;

var key = 0;

function httpGet(theUrl) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", theUrl, false); // false for synchronous request
    xmlHttp.send(null);
    if (xmlHttp.responseText == "File Not Found") {
        console.log("404 on server, error 404");
        return "{}"
    }

    return xmlHttp.responseText;
}

function get_temp() {
    //console.log("get temp");
    var value;


    if (DEBUG)
        value = '{"t0":"0.00","t1":"0.00","t2" :"0.00","t3":"0.00"}';
    else
        value = httpGet("/get_temp");

    var tmp = JSON.parse(value);

    document.getElementById('temp0').innerText = `Температура 1 > ${tmp.t0} C°`;
    document.getElementById('temp1').innerText = `Температура 2 > ${tmp.t1} C°`;
    document.getElementById('temp2').innerText = `Температура 3 > ${tmp.t2} C°`;
    document.getElementById('temp3').innerText = `Температура 4 > ${tmp.t3} C°`;

    console.log(value);
    console.log(tmp);
    console.log(document.getElementsByName('temp3').value);

}

function get_fq() {
    //console.log("get fq");
    if (DEBUG)
        value = '{"f0":"0","f1":"0"}';
    else
        value = httpGet("/get_fq");

    var tmp = JSON.parse(value);

    document.getElementById('fq0').innerText = `Мотор 1 > ${tmp.f0} Hz`;
    document.getElementById('fq1').innerText = `Мотор 2 > ${tmp.f1} Hz`;

    //console.log(value);
}

function set_temp(chen_num, value) {
    console.log("set temp");
    var request = "/set_temp?chenl=";
    request += chen_num;
    request += "&temp=";
    request += value;
    request += "&key=";
    request += key;

    var result;
    var result = httpGet(request);
    console.log("Request is: " + request + "\nResult is: " + result);
}

function set_fq(chen_num, frequency) {
    console.log("set fq");
    var request = "/set_fq?chenl=";
    request += chen_num;
    request += "&frequency=";
    request += frequency;
    request += "&key=";
    request += key;

    var result;
    var result = httpGet(request);
    console.log("Request is: " + request + "\nResult is: " + result);
}

//setInterval(get_temp, 10);
//setInterval(get_fq(), 10);

get_temp();
get_fq();

set_temp(1, 2);
set_fq(1, 2);