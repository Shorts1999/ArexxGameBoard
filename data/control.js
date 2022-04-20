let buttonIds = {
    UpArrow: { id: 10 },
    DownArrow: { id: 11 },
    LeftArrow: { id: 12 },
    RightArrow: { id: 13 },
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