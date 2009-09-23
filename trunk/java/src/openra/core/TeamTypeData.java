package openra.core;

public class TeamTypeData {

	private String name;
	private String data;
	
	public TeamTypeData(String string, String strToDecode) {
		this.name = string;
		this.data = strToDecode;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getData() {
		return data;
	}

	public void setData(String data) {
		this.data = data;
	}

}
