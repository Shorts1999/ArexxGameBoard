let buttonIds = {
    UpPad: { id: 10 },
    DownPad: { id: 11 },
    LeftPad: { id: 12 },
    RightPad: { id: 13 },
    ButtonA: { id: 20 },
    ButtonB: { id: 21 },
    ButtonStart: { id: 30 },
    ButtonSelect: { id: 31 },
}

function buttonHandler(element) {
    let xhr = new XMLHttpRequest();
    let requestBody = new FormData();

    requestBody.append("buttonId", buttonIds[element.srcElement.id].id)

    xhr.open("POST", "/button", true);
    xhr.send(requestBody);
}

for (const key in buttonIds) {
    let btn = document.getElementById(key);
    btn.onclick = (e) => buttonHandler(e);
}

setInterval((e) => {
    fetch("/update")
        .then(response => response.json())
        .then(responseJson => {
            document.getElementById("scoreText").innerText=`Score: ${responseJson["score"]}`;
            if(responseJson["state"]==true){ document.getElementById("gameOverText").innerText=`Game Over`;}
        })
}, 2000);