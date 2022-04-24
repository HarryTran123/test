//--------------------------------------------------------------------------------------------
// Tham chiếu thư viện
var MongoClient = require('mongodb').MongoClient;
// link kết nối đến database
var url = "mongodb+srv://dht11connector:connectMongoDB@cluster0.8zaaw.mongodb.net/DHT11?retryWrites=true&w=majority";
// tạo đối tượng và truyền dữ liệu qua url
var mongo = new MongoClient(url, { useNewUrlParser: true });
var fs = require('fs');
var url = require('url');
var http = require('http');
var querystring = require('querystring');
var db = []; //database
//---------------------------------------------------------------------------------------------
// function gửi yêu cầu(response) từ phía server hoặc nhận yêu cầu (request) của client gửi lên
function requestHandler(request, response) {

    // Giả sử địa chỉ nhận được http://192.168.1.7:8000/update?temp=30&humd=40
    var uriData = url.parse(request.url);
    var pathname = uriData.pathname;          // /update?
    var query = uriData.query;                // temp=30.5&hum=80
    var queryData = querystring.parse(query); // queryData.temp = 30.5, queryData.humd = 40
    //-----------------------------------------------------------------------------------------
    if (pathname == '/update') {
        var newData = {
            temp: queryData.temp,
            humd: queryData.humd,
            time: new Date()
        };
        db.push(newData);
        console.log(newData);
        // Kết nối đến Database
        mongo.connect((err, db) => {
            if (err) throw err;
            console.log("Kết nối thành công");
            // chọn database để sử dụng
            var dbo = db.db("DHT11");
            dbo.collection("data").insertOne(newData, (err, result) => {
                if (err) throw err;
                console.log("Thêm thành công");
                console.log(result);
                db.close();
            });
        });
        response.end();
        //-----------------------------------------------------------------------------------------
    } else if (pathname == '/get') {
        response.writeHead(200, {
            'Content-Type': 'application/json'
        });
        response.end(JSON.stringify(db));
        db = [];
        //-----------------------------------------------------------------------------------------
    } else {
        fs.readFile('./index.html', function (error, content) {
            response.writeHead(200, {
                'Content-Type': 'text/html'
            });
            response.end(content);
        });
    }
    //-----------------------------------------------------------------------------------------
}
var server = http.createServer(requestHandler);
server.listen(80);
console.log('Server listening on port 8000  url:  http://localhost:8000');
