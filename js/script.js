
document.addEventListener('DOMContentLoaded', function(){
    var count = 0
    function createParagraph(){
        count += 1
        let para = document.createElement('p');
        para.textContent = 'this is paragraph'+ count;
        document.body.appendChild(para);
    }

    let btns = document.querySelectorAll('button');
    for(var i=0; i<btns.length; i++){
        btns[i].addEventListener('click', createParagraph);
    }
});
