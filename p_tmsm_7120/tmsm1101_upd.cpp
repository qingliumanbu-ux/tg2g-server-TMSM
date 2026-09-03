/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2014-03-28
功能: 铁水罐使用变更
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"

//#include "ttmsm15.h"
//#include "ttmsm19.h"

/*<remark>=========================================================
/// <summary>
/// 铁水罐使用变更
/// <para>
/// 1.接收传入的铁包实绩信息；
/// 2.按情况修改TTMSM11，新增TTMSM15或TTMSM19；
/// </para>
/// <para>数据库表：TTMSM11(铁包基本信息表)      </para>
/// <para>数据库表：TTMSM15(铁包维修实绩表)      </para>
/// <para>数据库表：TTMSM19(铁包使用实绩表)      </para>
/// <para>主调用函数：前台TTMSM1101画面调用。   </para>
/// </summary>
/// <param name="bcls_rec">铁水罐使用变更信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm1101_upd)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm1101_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	CString sqlstr15 = "";
	CString sqlstr19 = "";
	
	/* 业务变量 */
	CString v_maint_type = "";	// 维修类型
	CString v_status_old = "";	// 原状态
	CString v_status_new = "";	// 目标状态
	CString v_remark = "";		// 维修
	CString v_maker = "";		// 操作人
	
	/* 实体类定义 */	
	CModel ttmsm11("TTMSM11");
	//CTTMSM15 ttmsm15(conn);
	//CTTMSM19 ttmsm19(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		//v_maker = s.userid;
		//Log::Trace("","tmsm1101_upd","v_maker [{0}]"		,(const char*)v_maker);
		////记录总条数
		//int	rows = bcls_rec->Tables[0].Rows.get_Count();
		//Log::Trace("","tmsm1101_upd","rows [{0}]" ,rows);

		//v_status_old = (CString)bcls_rec->Tables[1].Rows[0]["STATUS_OLD"].ToString().Trim();
		//v_status_new = (CString)bcls_rec->Tables[1].Rows[0]["STATUS_NEW"].ToString().Trim();	

		//Log::Trace("","tmsm1101_upd","v_status_old [{0}]"	,(const char*)v_status_old);
		//Log::Trace("","tmsm1101_upd","v_status_new [{0}]"	,(const char*)v_status_new);		

		//sqlstr = " SELECT * FROM TTMSM11 "
		//		" WHERE SM_UNIT_NO = @ttmsm11.SM_UNIT_NO "
		//		" AND IRON_LADLE_NO = @ttmsm11.SM_UNIT_NO "
		//		;

		//// 生成使用流水号
		//sqlstr19 = " SELECT nvl(MAX(USE_SEQ_NO),0) "
		//		" FROM TTMSM19 "
		//		" WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
		//		" AND IRON_LADLE_NO = @ttmsm19.IRON_LADLE_NO "
		//		;

		//// 铁水罐状态：01-可用 11-使用 21-维修 99-报废
		//// 维修类型：S-小修 M-中修 L-大修
		//
		//// 先读取原来的状态，修改TTMSM11的结束时刻等字段，记履历
		//// 再故去新的状态，修改TTMSM11的开始时刻等字段，（不用记履历

		//// 处理原状态
		//if(v_status_old.Trim() == "11") // 使用
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_old.Trim() == 11 ");

		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		cmd_inq.SetCommandText( sqlstr );
		//		cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
		//		cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
		//		cmd_inq.ExecuteReader();
		//		if	(cmd_inq.Read())
		//		{
		//			cmd_inq.Fetch(ttmsm11);//把数据都压在头文件里面
		//		}
		//		cmd_inq.Close();

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		ttmsm11.USAGE_ET		= datetimeNow;
		//		ttmsm11.LADLE_LIFE		= ttmsm11["LADLE_LIFE"].ToDecimal() + 1; // 没用IRON_LADLE_LIFE，这个就是纯粹+1
		//		ttmsm11.USE_REMARK		= v_remark;
		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"		,(const char*)ttmsm11["SM_UNIT_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USAGE_ST [{0}]"		,(const char*)ttmsm11["USAGE_ST"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USAGE_ET [{0}]"		,(const char*)ttmsm11["USAGE_ET"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.LADLE_LIFE [{0}]"		,ttmsm11["LADLE_LIFE"].ToDecimal());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USE_REMARK [{0}]"		,(const char*)ttmsm11["USE_REMARK"].ToString());
		//		
		//		cmd_inq.SetCommandText( sqlstr19 );
		//		cmd_inq.Parameters.Set("ttmsm19.SM_UNIT_NO"		, ttmsm11["SM_UNIT_NO"].ToString());
		//		cmd_inq.Parameters.Set("ttmsm19.IRON_LADLE_NO"	, ttmsm11["IRON_LADLE_NO"].ToString());
		//		cmd_inq.ExecuteReader();
		//		if ( cmd_inq.Read() )
		//		{
		//			ttmsm11["USE_SEQ_NO"] = cmd_inq.GetInt32(1)+1;
		//		}
		//		cmd_inq.Close();
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USE_SEQ_NO [{0}]",ttmsm11["USE_SEQ_NO"].ToDecimal());

		//		/*新增铁水包使用实绩表*/
		//		ttmsm19.CopyFrom(ttmsm11);
		//		ttmsm19.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
		//		ttmsm19.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */

		//		ttmsm19.TrimOrBlank();
		//		ttmsm19.Insert();

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		//ttmsm11["MIT_STATUS"] = "11"; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					//" ,MIT_STATUS "
		//					" ,USE_SEQ_NO "
		//					" ,IRON_LADLE_POS " //铁水包位置
		//					" ,MOLTIRON_WT "	//铁水重量
		//					" ,LADLE_LIFE "		//包龄
		//					//" ,IRON_LADLE_LIFE "		//包龄 //LADLE_LIFE
		//					" ,LADLE_ERA "		//包役
		//					" ,WORK_MAKER "		//工作层(耐材厂家)
		//					" ,PROD_DATE "		//生产日期
		//					" ,USAGE_ST "		//使用开始时间
		//					" ,USAGE_ET "		//使用结束时间
		//					" ,USE_REMARK ",	//使用备注 "
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//else if(v_status_old.Trim() == "21") // 维修
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_old.Trim() == 21 ");
		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		cmd_inq.SetCommandText( sqlstr );
		//		cmd_inq.Parameters.Set( "ttmsm11.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
		//		cmd_inq.Parameters.Set( "ttmsm11.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
		//		cmd_inq.ExecuteReader();
		//		if	(cmd_inq.Read())
		//		{
		//			cmd_inq.Fetch(ttmsm11);//把数据都压在头文件里面
		//		}
		//		cmd_inq.Close();

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		ttmsm11.REPAIR_END_TIME	= datetimeNow;
		//		ttmsm11["MAINTAIN_REMARK"] = v_remark;

		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"		,(const char*)ttmsm11["SM_UNIT_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.REPAIR_START_TIME [{0}]"	,(const char*)ttmsm11["REPAIR_START_TIME"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.REPAIR_END_TIME [{0}]",(const char*)ttmsm11["REPAIR_END_TIME"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.MAINTAIN_REMARK [{0}]",(const char*)ttmsm11["MAINTAIN_REMARK"].ToString());

		//		cmd_inq.SetCommandText( sqlstr15 );
		//		cmd_inq.Parameters.Set("ttmsm15.SM_UNIT_NO"		, ttmsm11["SM_UNIT_NO"].ToString());
		//		cmd_inq.Parameters.Set("ttmsm15.IRON_LADLE_NO"	, ttmsm11["IRON_LADLE_NO"].ToString());
		//		cmd_inq.ExecuteReader();
		//		if ( cmd_inq.Read() )
		//		{
		//			ttmsm11["REPAIR_SEQ_NO"] = cmd_inq.GetInt32(1)+1;
		//		}
		//		cmd_inq.Close();
		//		 Log::Trace("","tmsm1101_upd","ttmsm11.REPAIR_SEQ_NO [{0}]",ttmsm11["REPAIR_SEQ_NO"].ToDecimal());

		//		/*新增铁水包维修实绩表*/
		//		ttmsm15.CopyFrom(ttmsm11);
		//		ttmsm15.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
		//		ttmsm15.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */

		//		ttmsm15.TrimOrBlank();
		//		ttmsm15.Insert();

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		//ttmsm11["MIT_STATUS"] = "21"; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					//" ,MIT_STATUS "
		//					" ,REPAIR_SEQ_NO "
		//					" ,MAINT_TYPE "				//维修类型
		//					" ,REPAIR_START_TIME "		//维修开始时刻
		//					" ,REPAIR_END_TIME "		//维修结束时刻
		//					" ,MAINTAIN_REMARK ",		//维修备注
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//else if(v_status_old.Trim() == "01")
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_old.Trim() == 01 不用改表");
		//}

		//// 处理新状态
		//if(v_status_new.Trim() == "11") // 使用
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_new.Trim() == 11 ");
		//	v_remark	 = (CString)bcls_rec->Tables[1].Rows[0]["REMARK"].ToString().Trim();
		//	Log::Trace("","tmsm1101_upd","v_remark [{0}]"		,(const char*)v_remark);

		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		ttmsm11.USAGE_ST	= datetimeNow;
		//		ttmsm11.USAGE_ET	= " ";
		//		ttmsm11.USE_REMARK	= v_remark;

		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"	,(const char*)ttmsm11["SM_UNIT_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USAGE_ST [{0}]"	,(const char*)ttmsm11["USAGE_ST"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.USE_REMARK [{0}]"	,(const char*)ttmsm11["USE_REMARK"].ToString());

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		ttmsm11["MIT_STATUS"] = v_status_new; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,MIT_STATUS "
		//					" ,USAGE_ST "		//使用开始时刻
		//					" ,USAGE_ET "		//使用结束时刻
		//					" ,USE_REMARK ",	//使用备注
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//else if(v_status_new.Trim() == "21") //维修
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_new.Trim() == 21 ");

		//	v_maint_type = (CString)bcls_rec->Tables[1].Rows[0]["MAINT_TYPE"].ToString().Trim();
		//	Log::Trace("","tmsm1101_upd","v_maint_type [{0}]"	,(const char*)v_maint_type);
		//	v_remark	 = (CString)bcls_rec->Tables[1].Rows[0]["REMARK"].ToString().Trim();
		//	Log::Trace("","tmsm1101_upd","v_remark [{0}]"		,(const char*)v_remark);

		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		ttmsm11.REPAIR_START_TIME	= datetimeNow;
		//		ttmsm11.REPAIR_END_TIME		= " ";
		//		ttmsm11.MAINT_TYPE		= v_maint_type;
		//		ttmsm11["MAINTAIN_REMARK"] = v_remark;

		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"		,(const char*)ttmsm11["SM_UNIT_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.REPAIR_START_TIME [{0}]"	,(const char*)ttmsm11["REPAIR_START_TIME"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.MAINT_TYPE [{0}]"		,(const char*)ttmsm11["MAINT_TYPE"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.MAINTAIN_REMARK [{0}]",(const char*)ttmsm11["MAINTAIN_REMARK"].ToString());

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		ttmsm11["MIT_STATUS"] = v_status_new; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,MIT_STATUS "
		//					" ,MAINT_TYPE "				//维修类型
		//					" ,REPAIR_START_TIME "		//维修开始时刻
		//					" ,REPAIR_END_TIME "		//维修结束时刻
		//					" ,MAINTAIN_REMARK ",		//维修备注
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//else if(v_status_new.Trim() == "01")
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_new.Trim() == 01 ");

		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"		,(const char*)ttmsm11["SM_UNIT_NO"].ToString());

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		ttmsm11["MIT_STATUS"] = v_status_new; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,MIT_STATUS ",
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//else if(v_status_new.Trim() == "99")
		//{
		//	Log::Trace("","tmsm1101_upd","v_status_new.Trim() == 99 ");
		//	for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//	{
		//		/*重置TTMSM11*/
		//		ttmsm11.Reset();
		//		/*读取传入的参数*/
		//		ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//		// 判传入的参数是否有错
		//		if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}
		//		if(ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的铁水包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsm1101_upd");
		//		}

		//		ttmsm11.REC_REVISOR		= v_maker;
		//		ttmsm11["REC_REVISE_TIME"] = datetimeNow;
		//		Log::Trace("","tmsm1101_upd","ttmsm11.IRON_LADLE_NO [{0}]"	,(const char*)ttmsm11["IRON_LADLE_NO"].ToString());
		//		Log::Trace("","tmsm1101_upd","ttmsm11.SM_UNIT_NO [{0}]"		,(const char*)ttmsm11["SM_UNIT_NO"].ToString());

		//		Log::Trace("","tmsm1101_upd","ttmsm11.Update");
		//		ttmsm11["MIT_STATUS"] = v_status_new; // 01-可用 11-使用 21-维修 99-报废
		//		ttmsm11.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,MIT_STATUS ",
		//					" SM_UNIT_NO,IRON_LADLE_NO ");
		//	}
		//}
		//
		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	return doFlag;
}
