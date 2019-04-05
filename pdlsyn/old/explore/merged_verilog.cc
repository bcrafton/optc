#include "merge.h"
#include <fstream>
#include <iostream>

using namespace std;

int log2number(int num) {
    return (int)(ceil(log2(num)));
}

std::string num2binary(int count, int width) {
    int n = count;
    std::string output;
    for (int i = 0; i < width; i++) {
       if (n) {
           int bit = n%2;
           output += boost::lexical_cast<std::string>(bit);
       } else {
           output += "0";
       }
       n = n/2;
    }
    std::reverse(output.begin(), output.end());
    return output;
}

void verilog_generate(merged_protocol_t *protocol, std::ofstream& out) {

    // Module Declaration
    //-----------------------------

    //-- Name
    out << "module " << protocol->getName() << "(" << endl ;

    //-- I/O
    out << "    // Sys" << endl << "    //------------ " << endl;
    out << "    input wire " <<  protocol->getClockName() << ", "
                << endl;
    out << "    input wire " << protocol->getResetName() << ", "
            << endl << endl;


    out << "    // Inputs" << endl << "    //------------ " << endl;
    int num_outs = protocol->getNumberOfOutputs();
    for (int i = 0; i < num_outs; i++) {
        out << "    input wire [" << protocol->getOutputSize(i) - 1 << ":0]  " << protocol->getOutputName(i) << ", "
                << endl;
    }
    out << endl;
    out << "    // Outputs" << endl << "    //------------ " << endl;
    int num_ins = protocol->getNumberOfInputs();
    for (int i = 0; i < num_ins - 1; i++) {
        out << "    output wire [" << protocol->getInputSize(i) - 1 << ":0]  " << protocol->getInputName(i) << ", "
                << endl;
    }
    out << "    output wire [" << protocol->getInputSize(num_ins - 1) - 1 << ":0]  "
            << protocol->getInputName(num_ins - 1);

    out << ");" << endl << endl;


    //-- States localparam
    //-----------------------------
    int state_width = log2number(protocol->getBehavior()->size());
    int count = 0;
    std::pair<merged_graph_t::vertex_iterator, merged_graph_t::vertex_iterator> vertex_range = vertices((protocol->getBehavior())->g);
    for(merged_graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) 
    {
        merged_vertex_t* vertex = &(protocol->getBehavior()->g[*itr]);
        out << "localparam " << vertex->getName() << " = "
            << state_width << "'b" << num2binary(count, state_width) << ";"
            << endl;
        count++;
    }
    out << endl;

    //-- Transition matched wire
    //-----------------------------
    out << "reg transition_matched; // Async Reg" << endl;

    //-- Output State
    //-----------------------------
    out << "reg [" << (state_width - 1)
            << ":0] state; // Async Reg" << endl;
    out << "reg [" << (protocol->getWidthControlInputs() - 1)
                << ":0] output_state; // Reg" << endl << endl;

    //-- Next State
    //-----------------------------
    out << "reg [" << (state_width - 1)
                    << ":0] next_state; // Async Reg" << endl;
    out << "reg [" << (protocol->getWidthControlInputs() - 1)
                << ":0] next_output_state; // Reg" << endl << endl;

    // Assign outputs
    //-----------------------------
    int size = 0;
    int num_ctrl_ins =  protocol->getNumControlInputs();
    int input_width = protocol->getWidthControlInputs();
    for (int i = 0; i < num_ctrl_ins; i++) {
        int outSize = protocol->getControlInputWidth(i);
        out << "assign "<< protocol->getControlInputName(i) <<" = output_state["<< (input_width - 1) - size<<":" << input_width - size - outSize <<"];" << endl;
        size += outSize;
    }
    out << endl;

    //-- Input Vector
    //-----------------------------
    out << "wire [" << (protocol->getWidthControlOutputs() - 1)
            << ":0] input_vector;" << endl;
    out << "assign input_vector = {";
    for (int i = 0; i < protocol->getNumControlOutputs(); i++) {
        out << protocol->getControlOutputName(i);
        if (i<protocol->getNumControlOutputs()-1)
            out << ", ";
    }
    out << "};" << endl << endl;

    // -- For each State (current state), the set of conditions on non default transitions define changes to a next state
    //--------------------------------

    out << "always @(*) begin" << endl;
    out << "   if (!"<< protocol->getResetName() <<") begin" << endl;
//    out << "       transition_matched = 0;" << endl;
    out << "       next_state = "<< state_width << "'b" <<  num2binary(0, state_width) <<";" << endl;
    out << "   end else begin" << endl << endl;

//    //---- Transition Matched case
//    //----   - Generate a case to determine if we match a transition or not
//    out << "   casex({input_vector,output_state})" << endl;
//
//    for(merged_graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) 
//    {
//        merged_vertex_t* vertex = &(protocol->getBehavior()->g[*itr]);
//        std::string vertex_name = vertex->getName();
//        std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> edge_range = out_edges(*itr,protocol->getBehavior()->g);
//        for(merged_graph_t::out_edge_iterator itr = edge_range.first; itr != edge_range.second; itr++)
//        {
//            merged_edge_t e = protocol->getBehavior()->g[*itr];
//            if (protocol->getTotalOutputConditions(e)) {
//                out << "            " << "{" << protocol->getWidthControlOutputs() << "'b" << protocol->getOutputVectorCondition(e) <<"," << vertex_name << "}: transition_matched = 1;" << endl;
//            }
//        }
//    }
//
//    //-- Per default, transition is not matched
//    out << "            default: transition_matched = 0;" << endl;
//    out << "        endcase"<<endl <<  endl;

    //---- State setting case
    //---    - Generate a full_case to determine which state is the next one
    //---    - We can use a full_case because the final state decision is covered by the transition_matched signal
    out << "   next_state = " << protocol->getResetName()<< ";" << endl;
    out << "   casex({input_vector,output_state})" << endl;
    for(merged_graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) 
    {
        merged_vertex_t* vertex = &(protocol->getBehavior()->g[*itr]);
        std::string vertex_name = vertex->getName();
        std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> edge_range = out_edges(*itr,protocol->getBehavior()->g);
        for(merged_graph_t::out_edge_iterator itr = edge_range.first; itr != edge_range.second; itr++)
        {
            merged_edge_t e = protocol->getBehavior()->g[*itr];
            if(protocol->getTotalOutputConditions(e)) {
                vid_t targetVid = target(*itr, protocol->getBehavior()->g);
                merged_vertex_t target_vertex = protocol->getBehavior()->g[targetVid];
                std::string targetVertexName = target_vertex.getName();
                out << "            " << "{" << protocol->getWidthControlOutputs() << "'b" << protocol->getOutputVectorCondition(e) <<"," << vertex_name << "}: begin" << endl ;
                out <<"                next_state = "<< targetVertexName << ";" << endl;
                out <<"                next_output_state = "<< protocol->getWidthControlInputs() << "'b"<< protocol->getInputVectorCondition(e) << ";" << endl;
                out <<"                end" << endl;
            }
        }
    }

    out << "        endcase"<<endl <<  endl;
    out << "    end" << endl << endl;
    out << "end" << endl << endl;

    // Output State = Matched next state from case, or default value
    //----------------------
    out << "`ifdef ASYNC_RES" << endl;
    out << " always @(posedge " << protocol->getClockName().c_str()
            << " or negedge " << protocol->getResetName().c_str() << " ) begin"
            << endl;
    out << "`else" << endl;
    out << " always @(posedge " << protocol->getClockName().c_str()
            << ") begin" << endl;
    out << "`endif" << endl;
    out << "    if (!"<<protocol->getResetName() << ") begin" << endl;
    out << "        output_state <= "<< protocol->getWidthControlInputs() << "'b" << num2binary(0, protocol->getWidthControlInputs()) <<";" << endl;


    //---- Default transition if no match
    //-----------------------
//    out << "    end else if (!transition_matched) begin" << endl << endl;
//    out << "        case(output_state) // cadence full_case" << endl;
//    FOREACH_STATE(protocol)
//            FOREACH_STATE_STARTING_TRANSITIONS(state)
//                if (!transition->isDefault())
//                    continue;
//    out << "            "<< state->getName() <<" : output_state <= "<< transition->getEndState()->getName()  <<" ;" << endl;
//                break;
//
//            END_FOREACH_STATE_STARTING_TRANSITIONS
//    END_FOREACH_STATE
//
//
//    out << "        endcase" << endl;
//
//
    out << "    end else begin" << endl << endl;

    //---- Resulting state if match
    //--------------------------
    out << "        output_state <= next_output_state;" << endl;
    out << "        state <= next_state;" << endl;

    out << "    end" << endl;
    out << "end" << endl;

    // Module end
    //------------------
    out << "endmodule" << endl;



}

