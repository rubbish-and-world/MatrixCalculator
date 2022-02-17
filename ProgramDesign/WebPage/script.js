// count the number of existing matrix
    var cot = 0;

//--------------------------------------------------------- draw --------------------------------------------------------- 

    class Item {
            constructor(r , c, e){
                this.r = r;
                this.c = c;
                this.e = e;
            }
        }
    
    class CompressedMatrix{
        constructor(row , column , NonZero){
            this.row = row;
            this.column = column;
            this.NonZero = NonZero ;
            this.loarray = [];
        }
    }

    function createfromstring(s){
    
    let sp = s.replaceAll('\n' , ' ').split(' ');
    let filtered = sp.filter(function (el) { return el != ""; });
    let a = filtered.map(function(item) { return parseInt(item, 10); });

    for (const i in a){
        if (Number.isNaN(a[i])) return undefined ;
    }
    let no0len = a[2] * 3;
    let res = new CompressedMatrix(a[0] , a[1] , no0len);

    for ( var i = 3 ; i <= no0len ; i+=3){
        var index = i/3 - 1;
        it = new Item(a[i] , a[i+1] , a[i+2]);
        res.loarray.push(it);
    };

    return res ;
    }

    function draw(matrix , matrixID = "table" + String(cot)){
     
        let rowlen = matrix.row + 1;
        let collen = matrix.column + 1;

        let wrapper = document.createElement("div");
        wrapper.id = matrixID + "wrapper";
        wrapper.className = "matrix";
        let title = document.createElement("span");
        title.innerHTML = matrixID;
        title.className = "matrixheader";

        wrapper.appendChild(title);


        let newtable = document.createElement("table");
        newtable.id = matrixID;

        let tbody = newtable.createTBody();

        for ( var i = 0 ; i < rowlen ; i++){
            let newrow = document.createElement("tr");
            for ( var j = 0 ; j < collen ; j++){
                let cell = document.createElement("td");
                let cellvalue = 0 ;

                if ( i == 0 && j == 0){
                    cellvalue = "";
                    cell.className = "void";
                }
                else if ( i == 0 || j == 0){
                    cellvalue = i + j ;
                    cell.id = "Mindex";
                    cell.className = "matrixindex";
                }
                cell.innerHTML = cellvalue ;
                newrow.appendChild(cell);
            }
            tbody.appendChild(newrow);
        }

        
        for ( let cot = 0 ; cot < matrix.loarray.length ; cot ++ ){
            let it = matrix.loarray[cot];
            tbody.children[it.r+1].children[it.c+1].innerHTML = it.e;
        }

        wrapper.appendChild(newtable);

        let tail = document.getElementById("tail");
        document.body.insertBefore(wrapper , tail);
        
    }
//--------------------------------------------------------- draw --------------------------------------------------------- 
//--------------------------------------------------------- clear --------------------------------------------------------- 
    
    function destroyAll(){
        let names = ["table1" , "table2" , "addres" , "subres" , "mulres" , "tas1res" , "tas2res"];
        for (const n in names){
            let ele = document.getElementById(names[n] + "wrapper");
            if (ele){
                ele.remove();
            }
        }
    }

//--------------------------------------------------------- clear --------------------------------------------------------- 
    function send(data){
        const xhttp = new XMLHttpRequest();
        xhttp.open("POST" , "/test.html" , false);
        xhttp.send(data);
        if ( xhttp.status != 200){
            alert(xhttp.response);
            return ;
        }
        return xhttp.response;
    }

    function CreatDraw(responsetext , mid ){
        draw(createfromstring(responsetext) , mid );
    }


    function createMatrix(){
        if (cot >= 2){
            alert("Cannot create more than 2 Sparse Matrix!");
            return;
        }

        matrixdata = document.getElementById('userInput').value;


        if (createfromstring(matrixdata) == undefined ){
            alert("Invalid data, please check your input format");
            return ;
        }
        cot++;

        res = send("crt"  + String(cot) + "\n" + matrixdata );
        
        CreatDraw(res);
    }
    
    function check(){
        if (cot != 2){
            alert("Tow matrix needed to perform this operation");
            return false;
        }
        return true;
    }

    function add(){
        if(check()){
            res = send("add");
            let name = "addres";
            if (document.getElementById(name)){ alert("Operation already performed") ;return ; }
            CreatDraw(res, name);
        }
    }

    function sub(){
        if(check()){
           resp =  send("sub" );
           let name = "subres";
            if (document.getElementById(name)){ alert("Operation already performed") ;return ; }
           CreatDraw(resp , name);
        }
    }

    function mul(){
        if(check()){
            rhttp = send("mul");
            if (rhttp == undefined){
                return ;
            }
            let name = "mulres";
            if (document.getElementById(name)){ alert("Operation already performed") ;return ; }
            CreatDraw(rhttp , name);
        }
    }

    function tas1(){
        let name = "tas1res";
        if (document.getElementById(name)){ alert("Operation already performed") ;return ; }
        rhttp = send(name);
        CreatDraw(rhttp , name);
    }

    function tas2(){
        if (cot != 2){
            alert("Matrix 2 hasn't been created yet!");
            return;
        }
        let name = "tas2res" ; 
        if (document.getElementById(name)){ alert("Operation already performed") ;return ; }
        rhttp = send("tas2");
        CreatDraw(rhttp , name);
    }

    function clr(){
        send("clr");
        destroyAll();
        cot = 0;
    }

