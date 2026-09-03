/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-12
功能: 钢包维修实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm05.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包维修实绩新增
/// <para>
/// 1.接收传入的钢包维修实绩信息；
/// 2.新增钢包维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM05(钢包维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面维修实绩新增调用；TTMSM05画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的钢包维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm05_ins)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm05_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CString v_status = "";
	CString v_sys = "";
	
	/* 实体类定义 */
	CTTMSM05 ttmsm05(conn);
	CTTMSM01 ttmsm01(conn);
	CTTMSM01 ttmsm01_tmp(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		#if defined(_P1)
			v_sys = "P1";
		#endif

		#if defined(_P2)
			v_sys = "P2";
		#endif

		#if defined(_P3)
			v_sys = "P3";
		#endif

		Log::Trace("","tmsm05_ins","v_sys [{0}]" ,(const char*)v_sys);

		//记录总条数
		int	rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm05_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM02")
		{
			Log::Trace("","tmsm05_ins","rows [{0}]" ,rows);
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM05*/
				ttmsm05.Reset();
				/*读取传入的参数*/
				ttmsm05.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm05_ins","ttmsm05.LADLE_NO [{0}]"			,(const char*)ttmsm05.LADLE_NO);
				Log::Trace("","tmsm05_ins","ttmsm05.SM_UNIT_NO [{0}]"		,(const char*)ttmsm05.SM_UNIT_NO);
				Log::Trace("","tmsm05_ins","ttmsm05.REPAIR_START_TIME [{0}]",(const char*)ttmsm05.REPAIR_START_TIME);
				Log::Trace("","tmsm05_ins","ttmsm05.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm05.REPAIR_END_TIME);
				Log::Trace("","tmsm05_ins","ttmsm05.MAINT_TYPE [{0}]"		,(const char*)ttmsm05.MAINT_TYPE);

				// 判传入的参数是否有错			
				if(ttmsm05.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
				}
				if(ttmsm05.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
				}
				if(ttmsm05.REPAIR_START_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
				}
				if(ttmsm05.REPAIR_END_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
				}
				if(ttmsm05.MAINT_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
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

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm05.REPAIR_START_TIME,v_shift,v_group,conn );

				/*查询钢包维修实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM05 "
						 "  WHERE SM_UNIT_NO		= @ttmsm05.SM_UNIT_NO "
						 "  AND REPAIR_START_TIME	= @ttmsm05.REPAIR_START_TIME "
						 "  AND MAINT_TYPE			= @ttmsm05.MAINT_TYPE "
						 "  AND LADLE_NO			= @ttmsm05.LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm05.SM_UNIT_NO"		, ttmsm05.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm05.REPAIR_START_TIME"	, ttmsm05.REPAIR_START_TIME );
				cmd_inq.Parameters.Set( "ttmsm05.MAINT_TYPE"		, ttmsm05.MAINT_TYPE );
				cmd_inq.Parameters.Set( "ttmsm05.LADLE_NO"			, ttmsm05.LADLE_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm05_ins","count {0}",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"钢包维修实绩信息已存在");
					/*新增钢包维修实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
				}
				
				// 生成维修流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(REPAIR_SEQ_NO),0) "
							" FROM TTMSM05 "
							" WHERE SM_UNIT_NO	= @ttmsm05.SM_UNIT_NO "
							" AND LADLE_NO 		= @ttmsm05.LADLE_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm05.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm05.LADLE_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm05.REPAIR_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm05_ins","ttmsm05.REPAIR_SEQ_NO [{0}]",ttmsm05.REPAIR_SEQ_NO);

				/*新增钢包维修实绩表*/
				ttmsm05.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm05.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
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
				ttmsm05.Insert();
			}
		}		
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM05*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm05_ins","ttmsm01.LADLE_NO [{0}]"			,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm05_ins","ttmsm01.SM_UNIT_NO [{0}]"		,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm05_ins","ttmsm01.REPAIR_START_TIME [{0}]",(const char*)ttmsm01.REPAIR_START_TIME);
			Log::Trace("","tmsm05_ins","ttmsm01.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm01.REPAIR_END_TIME);
			Log::Trace("","tmsm05_ins","ttmsm01.MAINT_TYPE [{0}]"		,(const char*)ttmsm01.MAINT_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
			}
			if(ttmsm01.REPAIR_START_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
			}
			if(ttmsm01.REPAIR_END_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
			}
			if(ttmsm01.MAINT_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
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
					 "  AND REPAIR_START_TIME	= @ttmsm05.REPAIR_START_TIME "
					 "  AND MAINT_TYPE			= @ttmsm05.MAINT_TYPE "
					 "  AND LADLE_NO			= @ttmsm05.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm05.SM_UNIT_NO"		, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm05.REPAIR_START_TIME"	, ttmsm01.REPAIR_START_TIME );
			cmd_inq.Parameters.Set( "ttmsm05.MAINT_TYPE"		, ttmsm01.MAINT_TYPE );
			cmd_inq.Parameters.Set( "ttmsm05.LADLE_NO"			, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm05_ins","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"钢包维修实绩信息已存在");
				/*新增钢包维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm05_ins");
			}
			
			// 数据处理 
			// eg:小修的话，只有小修的日期、次数、包龄需要更改，中修大修永久层修的属性值还是用原来的
			sqlstr = " SELECT * FROM TTMSM01 "
					" WHERE SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
					" AND LADLE_NO 		= @ttmsm01.LADLE_NO "
					;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	, ttmsm01.LADLE_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				cmd_inq.Fetch(ttmsm01_tmp);//把数据都压在头文件里面
			}
			cmd_inq.Close();
			Log::Trace("","tmsm05_ins","ttmsm01_tmp");

			// 生成维修流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(REPAIR_SEQ_NO),0) "
						" FROM TTMSM05 "
						" WHERE SM_UNIT_NO	= @ttmsm05.SM_UNIT_NO "
						" AND LADLE_NO 		= @ttmsm05.LADLE_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm01.LADLE_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm01.REPAIR_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm05_ins","ttmsm01.REPAIR_SEQ_NO [{0}]",ttmsm01.REPAIR_SEQ_NO);

			/*新增钢包维修实绩表*/
			ttmsm05.CopyFrom(ttmsm01);
			ttmsm05.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm05.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm05.REC_REVISOR = " ";
			ttmsm05.REC_REVISE_TIME = " ";
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
			ttmsm05.Insert();

			//维修后属性清零
			//维修的时候 C0 
			//中修后 水口座砖和透气砖都清0 
			//大修后 座砖 透气砖 渣线 包龄都清0 
			//永久层同大修 包役+1 维修属性里的包龄等取包龄记录履历 修完后清0
			// 现在都按P3的写了
			if(v_sys.Trim() == "P1" || v_sys.Trim() == "P2" || v_sys.Trim() == "P3")
			{
				sqlstr =	" SELECT COUNT(1) "
						" FROM TTMSM05 "
						" WHERE SM_UNIT_NO	= @ttmsm05.SM_UNIT_NO "
						" AND LADLE_NO 		= @ttmsm05.LADLE_NO "
						" AND MAINT_TYPE 	= @ttmsm05.MAINT_TYPE "
						;

				if(ttmsm01.MAINT_TYPE.Trim() == "S")
				{
					Log::Trace("","tmsm05_ins","小修");
					cmd_inq.SetCommandText( sqlstr );
					cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
					cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm01.LADLE_NO);
					cmd_inq.Parameters.Set("ttmsm05.MAINT_TYPE"	, "S");
					cmd_inq.ExecuteReader();
					if ( cmd_inq.Read() )
					{
						ttmsm01.SMALL_REPAIR_NUM = cmd_inq.GetInt32(1);
					}
					cmd_inq.Close();

					Log::Trace("","tmsm05_ins","ttmsm01.SMALL_REPAIR_NUM [{0}]",ttmsm01.SMALL_REPAIR_NUM);
				}
				else if(ttmsm01.MAINT_TYPE.Trim() == "M")
				{
					Log::Trace("","tmsm05_ins","中修 水口座砖和透气砖 清零");
					
					cmd_inq.SetCommandText( sqlstr );
					cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
					cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm01.LADLE_NO);
					cmd_inq.Parameters.Set("ttmsm05.MAINT_TYPE"	, "M");
					cmd_inq.ExecuteReader();
					if ( cmd_inq.Read() )
					{
						ttmsm01.MIDDLE_REPAIR_NUM = cmd_inq.GetInt32(1);
					}
					cmd_inq.Close();
					Log::Trace("","tmsm05_ins","ttmsm01.MIDDLE_REPAIR_NUM [{0}]",ttmsm01.MIDDLE_REPAIR_NUM);
					//中修后 水口座砖和透气砖都清0 
					ttmsm01.NOZZLE_BRICK_TIMES = 0;
					ttmsm01.LADLE_BRICK_TIMES = 0;
				}
				else if(ttmsm01.MAINT_TYPE.Trim() == "L")
				{
					Log::Trace("","tmsm05_ins","中修 水口座砖和透气砖 清零");
					
					cmd_inq.SetCommandText( sqlstr );
					cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
					cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm01.LADLE_NO);
					cmd_inq.Parameters.Set("ttmsm05.MAINT_TYPE"	, "L");
					cmd_inq.ExecuteReader();
					if ( cmd_inq.Read() )
					{
						ttmsm01.BIG_REPAIR_NUM = cmd_inq.GetInt32(1);
					}
					cmd_inq.Close();
					Log::Trace("","tmsm05_ins","ttmsm01.BIG_REPAIR_NUM [{0}]",ttmsm01.BIG_REPAIR_NUM);
					//大修后 座砖 透气砖 渣线 包龄都清0  
					ttmsm01.NOZZLE_BRICK_TIMES = 0;
					ttmsm01.LADLE_BRICK_TIMES = 0;
					ttmsm01.SLAGLINE_TIMES = 0;
					ttmsm01.LADLE_LIFE = 0;
				}
				else if(ttmsm01.MAINT_TYPE.Trim() == "F")
				{
					Log::Trace("","tmsm05_ins","中修 水口座砖和透气砖 清零");
					
					cmd_inq.SetCommandText( sqlstr );
					cmd_inq.Parameters.Set("ttmsm05.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
					cmd_inq.Parameters.Set("ttmsm05.LADLE_NO"	, ttmsm01.LADLE_NO);
					cmd_inq.Parameters.Set("ttmsm05.MAINT_TYPE"	, "F");
					cmd_inq.ExecuteReader();
					if ( cmd_inq.Read() )
					{
						ttmsm01.FOREVER_REPAIR_NUM = cmd_inq.GetInt32(1);
					}
					cmd_inq.Close();
					Log::Trace("","tmsm05_ins","ttmsm01.FOREVER_REPAIR_NUM [{0}]",ttmsm01.FOREVER_REPAIR_NUM);
					//大修后 座砖 透气砖 渣线 包龄都清0  
					ttmsm01.NOZZLE_BRICK_TIMES = 0;
					ttmsm01.LADLE_BRICK_TIMES = 0;
					ttmsm01.SLAGLINE_TIMES = 0;
					ttmsm01.LADLE_LIFE = 0;
				}
			}
			//else if(v_sys.Trim() == "P2")
			//{
			//}
			//else if(v_sys.Trim() == "P3")
			//{
			//}

			sqlstr = "UPDATE TTMSM01";
			// 00-新包 01-可用 02-凉包 03-黑包 11-烘烤 21-使用 31-维修 32-粘钢 99-报废
			ttmsm01.LADLE_STATUS = "31"; 
			ttmsm01.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,LADLE_STATUS "
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
							" ,SLAGLINE_TIMES "			//渣线次数
							" ,SLIP_BOARD_USE_TIMES "	//滑板使用次数
							" ,NOZZLE_BRICK_TIMES "		//座砖
							" ,LADLE_BRICK_TIMES "		//透气砖
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