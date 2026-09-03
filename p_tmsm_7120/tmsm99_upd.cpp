/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2014-01-20
功能: 铁包包龄计算公式修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
//#include "ttmsm99.h"

/*<remark>=========================================================
/// <summary>
/// 铁包使用实绩修改
/// <para>
/// 1.接收传入的铁包包龄计算公式信息；
/// 2.修改铁包包龄计算公式信息；
/// </para>
/// <para>数据库表：TTMSM99(铁包包龄计算公式表)      </para>
/// <para>主调用函数：前台TTMSM11画面修改调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的铁包包龄计算公式信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm99_upd)

int f_tmsm99_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	//CString v_group = "";
	
	/* 实体类定义 */
	//CTTMSM99 ttmsm99(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		////记录总条数
		//int rows = bcls_rec->Tables[0].Rows.get_Count();
		//	//循环处理
		//for (int i = 0; i < rows ; i++ )
		//{
		//	// 取得单行传入信息 
		//	ttmsm99.Reset();
		//	ttmsm99.MergeFrom(bcls_rec->Tables[0].Rows[i]);
		//	ttmsm99.Print();

		//	// 判传入的参数是否有错	
		//	if(ttmsm99.MODEL_ID.Trim() == "")
		//	{
		//		sprintf	(s.msg , "传入的公式号不能为空") ;
		//		throw	CApplicationException(-1 , s.msg , "tmsm99_upd");
		//	}
		//				
		//	/*查询铁包使用实绩表*/
		//	sqlstr = "  SELECT COUNT(1) "
		//			 "  FROM TTMSM99 "
		//			 "  WHERE MODEL_ID	= @ttmsm99.MODEL_ID "
		//			 ;
		//	cmd_inq.SetCommandText( sqlstr );
		//	cmd_inq.Parameters.Set( "ttmsm99.MODEL_ID"	, ttmsm99.MODEL_ID );
		//	cmd_inq.ExecuteReader();
		//	count = 0 ;
		//	if(cmd_inq.Read())
		//	{
		//		count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
		//	}
		//	Log::Trace("","tmsm99_upd","count [{0}]",count );
		//	cmd_inq.Close();
		//	if	(count == 0)
		//	{
		//		sprintf	(s.msg ,"铁包包龄计算公式不存在");
		//		/*新增铁包使用实绩信息报错信息档*/
		//		throw	CApplicationException(-1 , s.msg , "tmsm99_upd");
		//	}
		//	
		//	//设置修改者信息
		//	ttmsm99.REC_REVISOR = s.userid;
		//	ttmsm99.REC_REVISE_TIME = datetimeNow;
		//	//根据传入信息更新铁包包龄计算公式表
		//	sqlstr = "UPDATE TTMSM99"; //用于捕获数据库操作异常情况
		//	ttmsm99.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,TPD_STD_PARA "   //倒罐标准罐系数
		//					" ,TPD_NSTD_PARA "   //倒罐非标准罐系数
		//					" ,MOLTIRON_WT_A "   //铁水来水重量A
		//					" ,MOLTIRON_WT_B "   //铁水来水重量B
		//					" ,MOLTIRON_PARA_X "  //铁水来水参数X
		//					" ,MOLTIRON_PARA_Y "   //铁水来水参数Y
		//					" ,MOLTIRON_PARA_Z "   //铁水来水参数Z
		//					" ,S_PARA "   //脱硫系数
		//					" ,NS_PARA ",	// 未脱硫系数
		//					//" ,REMARK " 							
		//					" MODEL_ID ");
		//}
		//
		////strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		//strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	

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