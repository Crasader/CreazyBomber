package com.giant.crazy.pay;

import net.youmi.android.offers.EarnPointsOrderList;
import net.youmi.android.offers.PointsReceiver;
import android.content.Context;
import android.content.Intent;

public class MyPointsReceiver extends PointsReceiver {

	@Override
	protected void onEarnPoints(Context arg0, EarnPointsOrderList arg1) {
		// TODO Auto-generated method stub

	}

	@Override
	protected void onViewPoints(Context context) {
		Intent i=new Intent(context,MyPointsReceiver.class);
		i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		context.startActivity(i);
	}

}