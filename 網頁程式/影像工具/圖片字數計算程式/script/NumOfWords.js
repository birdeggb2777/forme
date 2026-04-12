
var getById = function (id) { return document.getElementById(id) };

function onPasteImage() {
    var picture = getById("picture");
    picture.style["max-height"] = `calc(100vh - ${picture.offsetTop + 10}px)`;
}

window.onload = function () {
    var img = new Image();
    img.onload = function () {
        var canvas = getById("picture"), ctx = canvas.getContext('2d');
        canvas.width = img.width; canvas.height = img.height
        ctx.drawImage(img, 0, 0);
        onPasteImage();
    };
    img.src = "./舊版範例圖片/測試000002.png";
}

getById("countWordsBtn").onclick = function () {
    getById("countWordsBtn").disabled = true;
    getById("openFileBtn").disabled = true;
    setTimeout(function () {
        // 複製陣列
        var img = getById("picture"), ctx = img.getContext('2d');
        var imageData = ctx.getImageData(0, 0, img.width, img.height), data = imageData.data;
        var d1 = new Uint8ClampedArray(img.width * img.height * 4), d2 = new Array(img.height);
        for (var i = 0; i < img.height; i++) d2[i] = new Uint8ClampedArray(d1.buffer, i * img.width * 4, img.width * 4);
        for (var i = 0; i < data.length; i++) d1[i] = data[i];
        // 二值化
        for (var i = 0; i < data.length; i += 4) {
            var gray = 0.299 * data[i + 0] + 0.587 * data[i + 1] + 0.114 * data[i + 2];
            if (gray >= 128) d1[i + 0] = d1[i + 1] = d1[i + 2] = 255;
            else d1[i + 0] = d1[i + 1] = d1[i + 2] = 0;
        }
        // 前處理
        //橫向線條
        for (var h = 0; h < img.height; h++) {
            for (var w = 0; w < img.width * 4; w += 4) {
                if (!(d2[h][w + 0] == 255)) break;
                if (w == img.width * 4 - 4) {
                    for (var c = 0; c < img.width * 4; c += 4)d2[h][c + 0] = d2[h][c + 1] = d2[h][c + 2] = 128;
                    break
                }
            }
        }

        for (var w = 0; w < img.width * 4; w += 4) {
            for (var h = 1; h < img.height; h++) {
                if (d2[h][w + 0] == 255) {
                    d2[h][w + 0] = d2[h][w + 1] = d2[h][w + 2] = 128;
                } else {
                    for (; h < img.height; h++) {
                        if (d2[h - 1][w] == 128 && d2[h][w] == 255) break;
                    }
                }
            }
            for (var h = img.height - 1; h >= 1; h--) {
                if (d2[h][w + 0] == 255) {
                    d2[h][w + 0] = d2[h][w + 1] = d2[h][w + 2] = 128;
                } else {
                    for (; h >= 1; h--) {
                        if (d2[h - 1][w] == 128 && d2[h][w] == 255) break;
                    }
                }
            }
        }

        // 渲染
        var count = 0;
        for (var h = 4; h < img.height - 4; h++) {
            for (var w = 16; w < img.width * 4 - 16; w += 4) {
                if (d2[h + 0][w + 0] == 128 && d2[h + 0][w + 4] != 128 && d2[h + 0][w + 8] != 128 && d2[h + 0][w + 12] != 128 &&
                    d2[h + 1][w + 0] == 128 && d2[h + 1][w + 4] != 128 && d2[h + 1][w + 8] != 128 && d2[h + 1][w + 12] != 128 &&
                    d2[h + 2][w + 0] == 128 && d2[h + 2][w + 4] != 128 && d2[h + 2][w + 8] != 128 && d2[h + 2][w + 12] != 128 &&
                    d2[h + 3][w + 0] == 128 && d2[h + 3][w + 4] == 128 && d2[h + 3][w + 8] == 128 && d2[h + 3][w + 12] == 128) {
                    count++;
                    ctx.beginPath();
                    ctx.fillStyle = ctx.strokeStyle = "red";
                    ctx.font = `${((img.width / picture.clientWidth) * 16) | 0}px sans-serif`;
                    ctx.fillText("" + count, (w / 4) | 0, h);
                    w += 16;
                }
            }
        }

        setTimeout(function () {
            getById("countWordsBtn").disabled = false;
            getById("openFileBtn").disabled = false;
            alert("總字數：" + count);
        }, 10);
    }, 10);

}

getById("openFileBtn").onclick = function () {
    var fileElem = document.createElement("input");
    fileElem.setAttribute("type", "file");
    fileElem.setAttribute("multiple", "multiple");
    fileElem.onchange = function () {
        let reader = new FileReader();
        reader.readAsArrayBuffer(this.files[0]);
        reader.url = URL.createObjectURL(this.files[0]);
        reader.onloadend = function () {
            var img = new Image();
            img.onload = function () {
                var canvas = getById("picture"), ctx = canvas.getContext('2d');
                canvas.width = img.width; canvas.height = img.height;
                ctx.drawImage(img, 0, 0);
                onPasteImage();
            };
            img.src = this.url;
        }
    };
    fileElem.click();
}

window.addEventListener('paste', function (e) {
    // 1. 取得剪貼簿中的項目
    const items = (e.clipboardData || e.originalEvent.clipboardData).items;

    for (let i = 0; i < items.length; i++) {
        // 2. 檢查項目是否為圖片
        if (items[i].type.indexOf('image') !== -1) {
            const blob = items[i].getAsFile(); // 取得 Blob 檔案
            const reader = new FileReader();

            // 3. 讀取 Blob 並轉換為 DataURL
            reader.onload = function (event) {
                const img = new Image();
                img.onload = function () {
                    var canvas = getById("picture"), ctx = canvas.getContext('2d');
                    canvas.width = img.width; canvas.height = img.height
                    ctx.drawImage(img, 0, 0);
                    var data = ctx.getImageData(0, 0, img.width, img.height).data;
                    onPasteImage();
                };
                img.src = event.target.result;
            };
            reader.readAsDataURL(blob);
        }
    }
});
