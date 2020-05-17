const para = document.querySelector('p');

function updateContent(){
    let name = prompt('enter a name');
    para.textContent = name;
}

para.addEventListener('click', updateContent);