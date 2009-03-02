package openra.server;

import openra.core.Unit;

public class WalkAnimEvent extends UnitAnimEvent {
	public WalkAnimEvent(int p, Unit un, int dir, int layer) {
		super(p, un);
	}

	public void stop() {
	}

	public void run() {
	}

	public void changedir(int ndir) {
	}

	public void update() {
	}

	private void calcbaseimage() {
	}

	private boolean stopping;
	private Unit un;
	private int dir;
	private int istep;
	private int layer;
	private int baseimage;

}// WALKANIMEVENT_H

