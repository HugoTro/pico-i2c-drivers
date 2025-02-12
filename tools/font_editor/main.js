function createSlots() {
	var numberOfSlots = document.getElementById("slider").value;
	document.getElementById("amount").innerHTML = numberOfSlots;
	const insertionDiv = document.getElementById("font-editor");
	insertionDiv.textContent = "";
	insertionDiv.style.gridTemplateColumns = "repeat(" + numberOfSlots + ", 20px)"
	insertionDiv.style.gridTemplateRows = "repeat(" + numberOfSlots + ", 20px)"
	for (let i = 0; i < numberOfSlots; i++) {
		for (let j = 0; j < numberOfSlots; j++) {
			var element = document.createElement("input", {class: "checkbox", type: "checkbox"});
			element.setAttribute("class", "checkbox");
			element.setAttribute("type", "checkbox");
			element.setAttribute("id", i + "-" + j);
			insertionDiv.insertAdjacentElement("beforeend", element);
		}
	}
}
document.getElementById("slider").oninput = createSlots;
createSlots();