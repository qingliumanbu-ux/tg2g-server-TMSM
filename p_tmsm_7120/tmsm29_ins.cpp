/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 中间罐使用实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm29.h"
#include "ttmsm21.h"

/*<remark>=========================================================
/// <summary>
/// 中间罐使用实绩新增
/// <para>
/// 1.接收传入的中间罐使用实绩信息；
/// 2.新增中间罐使用实绩信息；
/// </para>
/// <para>数据库表：TTMSM29(中间罐使用实绩表)      </para>
/// <para>主调用函数：前台TTMSM21画面使用实绩新增调用；TTMSM23画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的中间罐使用实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm29_ins)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm29_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	CString v_group = "";
	CString v_shift = "";
	CString v_div = "";
	
	/* 实体类定义 */
	CTTMSM29 ttmsm29(conn);
	CTTMSM21 ttmsm21(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		//记录总条数
		int	rows = bcls_rec->Tables[0].Rows.get_Count(); 
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm29_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM23")
		{
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM29*/
				ttmsm29.Reset();
				/*读取传入的参数*/
				ttmsm29.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm29_ins","ttmsm29.TD_NO [{0}]"		,(const char*)ttmsm29.TD_NO);
				Log::Trace("","tmsm29_ins","ttmsm29.SM_UNIT_NO [{0}]"	,(const char*)ttmsm29.SM_UNIT_NO);
				Log::Trace("","tmsm29_ins","ttmsm29.ON_LINE_TIME [{0}]"	,(const char*)ttmsm29.ON_LINE_TIME);
				Log::Trace("","tmsm29_ins","ttmsm29.OFF_LINE_TIME [{0}]",(const char*)ttmsm29.OFF_LINE_TIME);

				//班次班组 shift_no shift_group
				//f_epep_get_shift_group("DEFAULT",ttmsm29.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错			
				if(ttmsm29.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
				}
				if(ttmsm29.TD_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的中间罐号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
				}
				if(ttmsm29.ON_LINE_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的使用起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
				}
				if(ttmsm29.OFF_LINE_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的使用结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
				}

				/*查询中间罐使用实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM29 "
						 "  WHERE SM_UNIT_NO	= @ttmsm29.SM_UNIT_NO "
						 "  AND ON_LINE_TIME	= @ttmsm29.ON_LINE_TIME "
						 "  AND TD_NO			= @ttmsm29.TD_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm29.SM_UNIT_NO"	, ttmsm29.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm29.ON_LINE_TIME"	, ttmsm29.ON_LINE_TIME );
				cmd_inq.Parameters.Set( "ttmsm29.TD_NO"			, ttmsm29.TD_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm29_ins","count {0}",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"中间罐使用实绩信息已存在");
					/*新增中间罐使用实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
				}
				
				// 生成使用流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
							" FROM TTMSM29 "
							" WHERE SM_UNIT_NO	= @ttmsm29.SM_UNIT_NO "
							" AND TD_NO 		= @ttmsm29.TD_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm29.SM_UNIT_NO"	, ttmsm29.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm29.TD_NO"		, ttmsm29.TD_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm29.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm29_ins","ttmsm29.USE_SEQ_NO [{0}]",ttmsm29.USE_SEQ_NO);

				/*新增中间罐使用实绩表*/
				ttmsm29.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm29.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
				//s.datetime是前台终端操作系统的时间，而不是服务器时间
				//if(ttmsm29.GRP_NO.Trim() == "")
				//{
				//	ttmsm29.GRP_NO = v_group;
				//}
				//if(ttmsm29.SHIFT_NO.Trim() == "")
				//{
				//	ttmsm29.SHIFT_NO = v_shift;
				//}

				ttmsm29.TrimOrBlank();
				ttmsm29.Insert();
			}
		}
		else if(v_div.Trim() == "TMSM21")
		{
			/*重置TTMSM21*/
			ttmsm21.Reset();
			/*读取传入的参数*/
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm29_ins","ttmsm21.TD_NO [{0}]"		,(const char*)ttmsm21.TD_NO);
			Log::Trace("","tmsm29_ins","ttmsm21.SM_UNIT_NO [{0}]"	,(const char*)ttmsm21.SM_UNIT_NO);
			Log::Trace("","tmsm29_ins","ttmsm21.ON_LINE_TIME [{0}]"	,(const char*)ttmsm21.ON_LINE_TIME);
			Log::Trace("","tmsm29_ins","ttmsm21.OFF_LINE_TIME [{0}]",(const char*)ttmsm21.OFF_LINE_TIME);

			// 判传入的参数是否有错			
			if(ttmsm21.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
			}
			if(ttmsm21.TD_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的中间罐号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
			}
			if(ttmsm21.ON_LINE_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的上线时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
			}
			//if(ttmsm21.OFF_LINE_TIME.Trim() == "")
			//{
			//	sprintf	(s.msg , "传入的下线时间不能为空") ;
			//	throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
			//}

			/*查询中间罐使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM29 "
					 "  WHERE SM_UNIT_NO	= @ttmsm29.SM_UNIT_NO "
					 "  AND ON_LINE_TIME	= @ttmsm29.ON_LINE_TIME "
					 "  AND TD_NO			= @ttmsm29.TD_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm29.SM_UNIT_NO"	, ttmsm21.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm29.ON_LINE_TIME"	, ttmsm21.ON_LINE_TIME );
			cmd_inq.Parameters.Set( "ttmsm29.TD_NO"			, ttmsm21.TD_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm29_ins","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"中间罐使用实绩信息已存在");
				/*新增中间罐使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm29_ins");
			}
			
			// 生成使用流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
						" FROM TTMSM29 "
						" WHERE SM_UNIT_NO	= @ttmsm29.SM_UNIT_NO "
						" AND TD_NO			= @ttmsm29.TD_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm29.SM_UNIT_NO"	, ttmsm21.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm29.TD_NO"		, ttmsm21.TD_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm21.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm29_ins","ttmsm21.USE_SEQ_NO [{0}]",ttmsm21.USE_SEQ_NO);

			/*新增中间罐使用实绩表*/
			ttmsm29.CopyFrom(ttmsm21);
			ttmsm29.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm29.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm29.REC_REVISOR = " ";
			ttmsm29.REC_REVISE_TIME = " ";

			ttmsm29.TrimOrBlank();
			ttmsm29.Insert();

			sqlstr = "UPDATE TTMSM21";
			ttmsm21.TD_STATUS = "11"; // 01-可用 11-使用 21-维修 99-报废
			ttmsm21.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,TD_STATUS "
							" ,USE_SEQ_NO "
							" ,CC_MACH_NO "		//连铸机号
							" ,LADLE_LIFE "		//包龄
							" ,TD_COVER_NO "	//中间包包盖号
							" ,BAKE_POS "		//烘烤位置
							" ,DRYING_ST "		//烘烤开始时刻
							" ,DRYING_ET "		//烘烤结束时刻
							" ,DRYING_REMARK "  //烘烤备注
							" ,PROD_DATE "		//生产日期
							" ,ONLINE_GROUP "   //上包班别
							" ,ONLINE_TIME "	//上包时间
							" ,ON_LINE_TIME "   //上线时间
							" ,OFF_LINE_TIME "  //下线时间
							" ,PONO "			//制造命令号
							" ,HEAT_NO "		//熔炼号
							" ,SM_PLAN_NO "		//炼钢计划号
							" ,CAST_NO "		//连连浇号(CAST号)
							" ,CAST_DIV_NO "	//CAST分割号
							" ,ST_NO "			//出钢记号
							" ,CAST_START_GROUP "	//浇铸开始班别
							" ,CAST_START_TIME "	//浇铸开始时刻
							" ,CAST_END_TIME "		//浇铸结束时刻
							" ,CAST_STOP_REASON "   //停浇原因
							" ,INSPECT_MAKER "		//检验人员
							" ,SEN_UPPER_MANU "		//上水口厂家
							" ,QUICK_CHANGE_MANU "  //快换机构厂家
							" ,WORK_MAKER "		//工作层(耐材厂家)
							" ,STAB_MANU "		//稳流器厂家
							" ,STRIKE_MANU "	//冲击板厂家
							" ,STOPPER_MANU "   //塞棒厂家
							" ,NOZZLE_BRICK_MANU "	//水口座砖厂家
							" ,SUBM_NOZZLE_MANU "   //浸入式水口厂家
							" ,SUBM_NOZZLE_TIME "   //浸入式水口使用寿命
							" ,LADLE_SHROUD_MANU "  //钢包保护套管厂家
							" ,LADLE_SHROUD_TIME "  //钢包保护套管使用寿命
							" ,DEV_REMARK_1 "   //设备备注1
							" ,USE_REMARK "   ,	//使用备注
							" SM_UNIT_NO,TD_NO ");
		}
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