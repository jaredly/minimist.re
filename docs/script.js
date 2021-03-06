
var listenForTypes = () => {
  var typeViewer = document.createElement('div')
  typeViewer.className = 'type-viewer'
  document.body.appendChild(typeViewer)
  ;[].forEach.call(document.querySelectorAll('pre.code'), el => {
    el.addEventListener('mousemove', evt => {
      typeViewer.style.top = evt.pageY + 'px'
      typeViewer.style.left = evt.pageX + 'px'
    });
    el.addEventListener('mouseover', evt => {
      if (evt.target.getAttribute('data-type')) {
        evt.target.classList.add('type-hovered')
        typeViewer.textContent = evt.target.getAttribute('data-type')
        typeViewer.style.display = 'block'
      }
    })
    el.addEventListener('mouseout', evt => {
      if (evt.target.getAttribute('data-type')) {
        evt.target.classList.remove('type-hovered')
        typeViewer.style.display = 'none'
      }
    })
  })
}

var checkHash = () => {
  if (!window.shouldCheckHashes) {
    return
  }
  var id = window.location.hash.slice(1)
  if (id && !document.getElementById(id)) {
    document.getElementById("error-message").style.display = 'block'
    var parts = id.split('-')
    document.querySelector('#error-message span').textContent = parts[0]
    document.querySelector('#error-message code').textContent = parts[1]
  } else {
    document.getElementById("error-message").style.display = 'none'
  }
}
window.onload = () => {
  checkHash()
  var expander = document.querySelector('.sidebar-expander')
  expander.onclick = () => {
    var sidebar = document.querySelector('.sidebar');
    if (sidebar.classList.contains('expanded')) {
      sidebar.classList.remove('expanded')
      expander.textContent = 'Show navigation'
    } else {
      sidebar.classList.add('expanded')
      expander.textContent = 'Hide navigation'
    }
  }
  listenForTypes();
}
window.onhashchange = checkHash
