/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-12
功能: 钢包维修实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm05.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包维修实绩修改
/// <para>
/// 1.接收传入的钢包维修实绩信息；
/// 2.修改钢包维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM05(钢包维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面维修实绩修改调用；TTMSM02画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的钢包维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm05_upd)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm05_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM05 ttmsm05(conn);
	CTTMSM01 ttmsm01(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm05_upd","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM02")
		{
			Log::Trace("","tmsm05_upd","rows [{0}]" ,rows);
			//循环处理
			for (int i = 0; i < rows ; i++ )
			{
				// 取得单行传入信息 
				ttmsm05.Reset();
				ttmsm05.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				Log::Trace("","tmsm05_upd","ttmsm05.REPAIR_SEQ_NO [{0}]"	,ttmsm05.REPAIR_SEQ_NO);
				Log::Trace("","tmsm05_upd","ttmsm05.LADLE_NO [{0}]"			,(const char*)ttmsm05.LADLE_NO);
				Log::Trace("","tmsm05_upd","ttmsm05.SM_UNIT_NO [{0}]"		,(const char*)ttmsm05.SM_UNIT_NO);
				Log::Trace("","tmsm05_upd","ttmsm05.REPAIR_START_TIME [{0}]",(const char*)ttmsm05.REPAIR_START_TIME);
				Log::Trace("","tmsm05_upd","ttmsm05.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm05.REPAIR_END_TIME);
				Log::Trace("","tmsm05_upd","ttmsm05.MAINT_TYPE [{0}]"		,(const char*)ttmsm05.MAINT_TYPE);

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm05.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				if(ttmsm05.REPAIR_SEQ_NO <= 0)
				{
					sprintf	(s.msg , "传入的维修流水号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				if(ttmsm05.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				if(ttmsm05.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				if(ttmsm05.REPAIR_START_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				if(ttmsm05.REPAIR_END_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				if(ttmsm05.MAINT_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}

				if(ttmsm05.BIG_REPAIR_DATE.Trim() != "")
				{
					ttmsm05.BIG_REPAIR_DATE = ttmsm05.BIG_REPAIR_DATE.Substring(0,8);
				}
				if(ttmsm05.MIDDLE_REPAIR_DATE.Trim() != "")
				{
					ttmsm05.MIDDLE_REPAIR_DATE = ttmsm05.MIDDLE_REPAIR_DATE.Substring(0,8);
				}
				if(ttmsm05.SMALL_REPAIR_DATE.Trim() != "")
				{
					ttmsm05.SMALL_REPAIR_DATE = ttmsm05.SMALL_REPAIR_DATE.Substring(0,8);
				}
				if(ttmsm05.FOREVER_REPAIR_DATE.Trim() != "")
				{
					ttmsm05.FOREVER_REPAIR_DATE = ttmsm05.FOREVER_REPAIR_DATE.Substring(0,8);
				}
				
				/*查询钢包维修实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM05 "
						 "  WHERE REPAIR_SEQ_NO	= @ttmsm05.REPAIR_SEQ_NO "
						 "  AND SM_UNIT_NO		= @ttmsm05.SM_UNIT_NO "
						 "  AND LADLE_NO		= @ttmsm05.LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm05.REPAIR_SEQ_NO"	, ttmsm05.REPAIR_SEQ_NO );
				cmd_inq.Parameters.Set( "ttmsm05.LADLE_NO"		, ttmsm05.LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm05.SM_UNIT_NO"	, ttmsm05.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				Log::Trace("","tmsm05_upd","count [{0}]",count );
				cmd_inq.Close();
				if	(count == 0)
				{
					sprintf	(s.msg ,"钢包维修实绩信息不存在");
					/*新增钢包维修实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
				}
				
				//设置修改者信息
				ttmsm05.REC_REVISOR = s.userid;
				ttmsm05.REC_REVISE_TIME = datetimeNow;

				if(ttmsm05.OPERATER.Trim() == "")
				{
					ttmsm05.OPERATER = s.userid;
				}
				if(ttmsm05.GRP_NO.Trim() == "")
				{
					ttmsm05.GRP_NO = v_group;
				}
				if(ttmsm05.SHIFT_NO.Trim() == "")
				{
					ttmsm05.SHIFT_NO = v_shift;
				}
				//根据传入信息更新钢包维修实绩表
				sqlstr = "UPDATE TTMSM05"; //用于捕获数据库操作异常情况
				ttmsm05.Update(	" REC_REVISOR "
								" ,REC_REVISE_TIME "
								" ,MAINT_TYPE "				//维修类型
								" ,REPAIR_START_TIME "		//维修开始时刻
								" ,REPAIR_END_TIME "		//维修结束时刻
								" ,BIG_REPAIR_NUM "			//大修次数
								" ,BIG_REPAIR_DATE "		//大修日期
								" ,BIG_REPAIR_USE_NUM "		//大修包龄
								" ,MIDDLE_REPAIR_NUM "		//中修次数
								" ,MIDDLE_REPAIR_DATE "		//中修日期
								" ,MIDDLE_REPAIR_USE_NUM "	//中修包龄
								" ,SMALL_REPAIR_DATE "		//小修日期
								" ,SMALL_REPAIR_NUM "		//小修次数
								" ,SMALL_REPAIR_USE_NUM "	//小修后使用次数
								" ,FOREVER_REPAIR_NUM "		//永久层修理次数
								" ,FOREVER_REPAIR_DATE "	//永久层修理日期
								" ,FOREVER_REPAIR_USE_NUM "	//永久层修理后使用次数
								" ,FOREVER_FAY "			//永久层厂家
								" ,MAINTAIN_REMARK ",		//维修备注
								" REPAIR_SEQ_NO,SM_UNIT_NO,LADLE_NO ");
				}
		}
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM05*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm05_upd","ttmsm01.LADLE_NO [{0}]"			,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm05_upd","ttmsm01.SM_UNIT_NO [{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm05_upd","ttmsm01.REPAIR_START_TIME [{0}]",(const char*)ttmsm01.REPAIR_START_TIME);
			Log::Trace("","tmsm05_upd","ttmsm01.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm01.REPAIR_END_TIME);
			Log::Trace("","tmsm05_upd","ttmsm01.MAINT_TYPE [{0}]"		,(const char*)ttmsm01.MAINT_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}
			if(ttmsm01.REPAIR_START_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}
			if(ttmsm01.REPAIR_END_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}
			if(ttmsm01.MAINT_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}

			if(ttmsm01.BIG_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.BIG_REPAIR_DATE = ttmsm01.BIG_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.MIDDLE_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.MIDDLE_REPAIR_DATE = ttmsm01.MIDDLE_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.SMALL_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.SMALL_REPAIR_DATE = ttmsm01.SMALL_REPAIR_DATE.Substring(0,8);
			}
			if(ttmsm01.FOREVER_REPAIR_DATE.Trim() != "")
			{
				ttmsm01.FOREVER_REPAIR_DATE = ttmsm01.FOREVER_REPAIR_DATE.Substring(0,8);
			}

			//班次班组 shift_no shift_group
			f_epep_get_shift_group("SM",ttmsm01.REPAIR_START_TIME,v_shift,v_group,conn );

			/*查询钢包维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM05 "
					 "  WHERE SM_UNIT_NO		= @ttmsm05.SM_UNIT_NO "					 
					 //"  AND MAINT_TYPE			= @ttmsm05.MAINT_TYPE "
					 "  AND REPAIR_SEQ_NO		= @ttmsm05.REPAIR_SEQ_NO "
					 "  AND LADLE_NO			= @ttmsm05.LADLE_NO "

					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm05.SM_UNIT_NO"		, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm05.REPAIR_SEQ_NO"		, ttmsm01.REPAIR_SEQ_NO );
			//cmd_inq.Parameters.Set( "ttmsm05.MAINT_TYPE"		, ttmsm01.MAINT_TYPE );
			cmd_inq.Parameters.Set( "ttmsm05.LADLE_NO"			, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm05_upd","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count == 0)
			{
				sprintf	(s.msg ,"钢包维修实绩信息不存在");
				/*新增钢包维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm05_upd");
			}
			
			/*修改钢包维修实绩表*/
			ttmsm05.CopyFrom(ttmsm01);
			//ttmsm05.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			//ttmsm05.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm05.REC_REVISOR = s.userid;
			ttmsm05.REC_REVISE_TIME = datetimeNow;
			//班次班组 shift_no shift_group

			//s.datetime是前台终端操作系统的时间，而不是服务器时间
			if(ttmsm05.OPERATER.Trim() == "")
			{
				ttmsm05.OPERATER = s.userid;
			}
			if(ttmsm05.GRP_NO.Trim() == "")
			{
				ttmsm05.GRP_NO = v_group;
			}
			if(ttmsm05.SHIFT_NO.Trim() == "")
			{
				ttmsm05.SHIFT_NO = v_shift;
			}
			ttmsm05.TrimOrBlank();
			sqlstr = "UPDATE TTMSM05"; //用于捕获数据库操作异常情况
			ttmsm05.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,MAINT_TYPE "				//维修类型
							" ,REPAIR_START_TIME "		//维修开始时刻
							" ,REPAIR_END_TIME "		//维修结束时刻
							" ,BIG_REPAIR_NUM "			//大修次数
							" ,BIG_REPAIR_DATE "		//大修日期
							" ,BIG_REPAIR_USE_NUM "		//大修包龄
							" ,MIDDLE_REPAIR_NUM "		//中修次数
							" ,MIDDLE_REPAIR_DATE "		//中修日期
							" ,MIDDLE_REPAIR_USE_NUM "	//中修包龄
							" ,SMALL_REPAIR_DATE "		//小修日期
							" ,SMALL_REPAIR_NUM "		//小修次数
							" ,SMALL_REPAIR_USE_NUM "	//小修后使用次数
							" ,FOREVER_REPAIR_NUM "		//永久层修理次数
							" ,FOREVER_REPAIR_DATE "	//永久层修理日期
							" ,FOREVER_REPAIR_USE_NUM "	//永久层修理后使用次数
							" ,FOREVER_FAY "			//永久层厂家
							" ,MAINTAIN_REMARK ",		//维修备注
							" REPAIR_SEQ_NO,SM_UNIT_NO,LADLE_NO ");

			sqlstr = "UPDATE TTMSM01";
			ttmsm01.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,REPAIR_SEQ_NO "
							" ,MAINT_TYPE "				//维修类型
							" ,REPAIR_START_TIME "		//维修开始时刻
							" ,REPAIR_END_TIME "		//维修结束时刻
							" ,BIG_REPAIR_NUM "			//大修次数
							" ,BIG_REPAIR_DATE "		//大修日期
							" ,BIG_REPAIR_USE_NUM "		//大修包龄
							" ,MIDDLE_REPAIR_NUM "		//中修次数
							" ,MIDDLE_REPAIR_DATE "		//中修日期
							" ,MIDDLE_REPAIR_USE_NUM "	//中修包龄
							" ,SMALL_REPAIR_DATE "		//小修日期
							" ,SMALL_REPAIR_NUM "		//小修次数
							" ,SMALL_REPAIR_USE_NUM "	//小修后使用次数
							" ,FOREVER_REPAIR_NUM "		//永久层修理次数
							" ,FOREVER_REPAIR_DATE "	//永久层修理日期
							" ,FOREVER_REPAIR_USE_NUM "	//永久层修理后使用次数
							" ,FOREVER_FAY "			//永久层厂家
							" ,MAINTAIN_REMARK "		//维修备注
							// 每录入一次大修实绩，渣线、滑板、包龄等所有的次数为0
							" ,SLAGLINE_TIMES "			//渣线次数
							" ,SLIP_BOARD_USE_TIMES "	//滑板使用次数							
							" ,LADLE_LIFE ",			//包龄
							" SM_UNIT_NO,LADLE_NO ");
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